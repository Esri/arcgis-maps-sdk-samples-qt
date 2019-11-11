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
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url outputMapPackage: System.temporaryFolder.url
    property var preplannedMapAreaList: null
    property var preplannedArea: null
    property var path: null
    property var mapExists: false
    property var loadedLayersCount: null
//    property var fileExists: false

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

//                offlineMapTask.preplannedMapAreaList.get(0).loadStatusChanged.connect(function (){
//                });
                //needs to be loaded before we can use it to check for the file. Need to rethink logic.

                for (let i = 0; i < offlineMapTask.preplannedMapAreaList.count; i++) {
//                    offlineMapTask.preplannedMapAreaList.get(i).loadStatusChanged.connect(checkAllLayersLoaded(offlineMapTask.preplannedMapAreaList.get(i)));
                    offlineMapTask.preplannedMapAreaList.get(i).load();
//                    offlineMapTask.preplannedMapAreaList.get(i).contentItems;
                }

            }

            onCreateDefaultDownloadPreplannedOfflineMapParametersStatusChanged: {
                if (createDefaultDownloadPreplannedOfflineMapParametersStatus !== Enums.TaskStatusCompleted)
                    return;

                createDefaultDownloadPreplannedOfflineMapParametersResult.updateMode = Enums.PreplannedUpdateModeNoUpdates;
                let result = createDefaultDownloadPreplannedOfflineMapParametersResult;
                path = outputMapPackage + "/11" + result.preplannedMapArea.portalItem.title + ".mmpk";
                console.log(path);


                fileFolder.url = path;
//                console.log(fileFolder.path);

                if( fileFolder.exists) {
//                    console.log("exists");
                    var mmpk = ArcGISRuntimeEnvironment.createObject("MobileMapPackage", { path: path });
                    mmpk.loadStatusChanged.connect(function () {
                        if( loadStatus !== Enums.LoadStatusLoaded )
                            return;
                        // loaded twice? first time not ready
//                        console.log(loadStatus);

                        if (mmpk.maps.length < 1)
                            return;
                        console.log("length - " + mmpk.maps.length);
                        mapView.map = mmpk.maps[0];
                    });
                    mmpk.load();
                    return;
                }

                var job = offlineMapTask.downloadPreplannedOfflineMapWithParameters(createDefaultDownloadPreplannedOfflineMapParametersResult, path);

                job.jobStatusChanged.connect(function () {
                    if( job.jobStatus !== Enums.JobStatusSucceeded)
                        return;

                    console.log("job status: %1 - Succeeded".arg(job.jobStatus));

                    mapView.map = job.result.offlineMap;

                });

                job.progressChanged.connect(function () {
                   console.log(job.progress);
                });

                job.start();
                console.log("job started");
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

                onActivated: {
                    if (offlineMapTask.preplannedMapAreaList.count <= 0)
                        return;

                    if (offlineMapTask.preplannedMapAreaList.get(currentIndex).loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    path = outputMapPackage + "/11" + offlineMapTask.preplannedMapAreaList.get(currentIndex).portalItem.title + ".mmpk";
                    fileFolder.url = path;

                    if (fileFolder.exists) {
                        mapExists = true;
                    } else {
                        mapExists = false;
                    }
                }

                onModelChanged: {
                    if( model )
                        console.log(model.itemTitle);
                }
            }

            Button {
                id: downloadOrView
//                text: qsTr("Download preplanned area")
//                text: fileFolder.exists ? qsTr("View preplanned area") : qsTr("Download preplanned area")
                text: mapExists ? qsTr("View preplanned area") : qsTr("Download preplanned area")
                onClicked: {
                    preplannedArea = offlineMapTask.preplannedMapAreaList.get(preplannedCombo.currentIndex);
                    offlineMapTask.createDefaultDownloadPreplannedOfflineMapParameters(preplannedArea);

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

    FileFolder {
        id: fileFolder
    }

    function checkAllLayersLoaded(loadingLayer) {

        if (loadingLayer.loadStatus != Enums.LoadStatusLoaded)
            return;

        loadedLayersCount++;

        if(loadedLayersCount != offlineMapTask.preplannedMapAreaList.count)
            return;

        mapExists = true;

    }

}
