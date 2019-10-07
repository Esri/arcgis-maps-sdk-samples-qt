// [WriteFile Name=DownloadPreplannedMap, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.7

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var preplannedMapAreaList: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: onlineMap
            item: portalItem
        }

        Portal {
            id: portal
            url: "https://www.arcgis.com"
        }

        PortalItem {
            id: portalItem
            portal: portal
            itemId: "acc027394bc84c2fb04d1ed317aac674"

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                offlineMapTask.load();
            }
        }

        GraphicsOverlay {
            id: graphicsOverlay
            renderer: SimpleRenderer {
                symbol: SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "red"
                    width: 5
                }
            }
        }

        OfflineMapTask {
            id: offlineMapTask
            onlineMap: onlineMap

            onErrorChanged: console.log(error.message, error.additionalMessage);

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                offlineMapTask.preplannedMapAreas();
            }

            onPreplannedMapAreasStatusChanged: {
                if( preplannedMapAreasStatus !== Enums.TaskStatusCompleted)
                    return;

//                preplannedMapAreaList = offlineMapTask.preplannedMapAreaList;
//                preplannedlist.model = offlineMapTask.preplannedMapAreaList;
                preplannedCombo.model = offlineMapTask.preplannedMapAreaList;

//                console.log(preplannedlist.count);
            }
        }





        Component.onCompleted: {
            portalItem.load();
        }
    }

    Rectangle {
        id: buttonBackground
        anchors {
            left: parent.left
            top: parent.top
            margins: 3
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        // catch mouse signals from propagating to parent
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            Text {
                id: name
                text: qsTr("Preplanned Map Areas:")
                color: "white"
                Layout.alignment: Qt.AlignLeft

            }

            Repeater {
                id: preplannedlist
                model: null
                delegate: Row {
                    Rectangle {
                        width: buttonBackground.width
                        height: childrenRect.height
                        color: "yellow"
                        // catch mouse signals from propagating to parent
                        MouseArea {
                            anchors.fill: parent
                            onClicked: console.log(myItem.text)
                            onWheel: wheel.accepted = true
                        }
                        Text {
                            id: myItem
                            text: itemTitle
                            color: "white"
                        }
                    }
                }
            }

            ListView {

            }

            ComboBox {
                id: preplannedCombo
                model: null
                textRole: "itemTitle"
            }

            Button {
                text: qsTr("Download")
                onClicked: {



//                    console.log("width %1 - height %2".arg(preplannedlist.width).arg(preplannedlist.height));
                }
            }
            Text {
                id: name1
                text: qsTr("Downloads (deleted on exit):")
                color: "white"
                Layout.alignment: Qt.AlignLeft
            }

            Repeater {
                id: appliedmaps
                model: null
                delegate: Row {
                    Text {
                        text: itemTitle
                        color: "white"
                    }
                }
            }
        }
    }
}
