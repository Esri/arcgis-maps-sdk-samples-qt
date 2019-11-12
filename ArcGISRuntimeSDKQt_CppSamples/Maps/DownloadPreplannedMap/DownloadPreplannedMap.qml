// [WriteFile Name=DownloadPreplannedMap, Category=Maps]
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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DownloadPreplannedMapSample {
        id: model
        mapView: view
    }

//    Rectangle {
//        id: buttonBackground
//        anchors {
//            left: parent.left
//            top: parent.top
//            margins: 3
//        }
//        width: childrenRect.width
//        height: childrenRect.height
//        color: "#000000"
//        opacity: .8
//        radius: 5

//        // catch mouse signals from propagating to parent
//        MouseArea {
//            anchors.fill: parent
//            onClicked: mouse.accepted = true
//            onWheel: wheel.accepted = true
//        }

//        ColumnLayout {

//        }
//    }

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
                model: model.preplannedModel
                textRole: "itemTitle"

                onActivated: {
//                    if (offlineMapTask.preplannedMapAreaList.count <= 0)
//                        return;

//                    if (offlineMapTask.preplannedMapAreaList.get(currentIndex).loadStatus !== Enums.LoadStatusLoaded)
//                        return;

//                    path = outputMapPackage + "/11" + offlineMapTask.preplannedMapAreaList.get(currentIndex).portalItem.title + ".mmpk";
//                    fileFolder.url = path;

//                    if (fileFolder.exists) {
//                        mapExists = true;
//                    } else {
//                        mapExists = false;
//                    }

                    model.checkIfMapExists(preplannedCombo.currentIndex);
                }
            }

            Button {
                id: downloadOrView
                text: model.mapExists ? qsTr("View preplanned area") : qsTr("Download preplanned area")
//                text: "Download Preplanned Area"
                onClicked: {
                    model.downloadMapArea(preplannedCombo.currentIndex);
                }
            }
//            Text {
//                id: name1
//                text: qsTr("Downloads (deleted on exit):")
//                color: "white"
//                Layout.alignment: Qt.AlignLeft
//            }

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

    FileFolder {
        id: fileFolder
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: model.busy
    }
}
