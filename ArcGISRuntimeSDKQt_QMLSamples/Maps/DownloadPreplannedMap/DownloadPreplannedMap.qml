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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url outputMapPackage: System.temporaryFolder.url + "/ArcGIS"
    property var preplannedMapAreaList: null
    property var preplannedArea: null
    property var path: null
    property var mapExists: false
    property var viewingOnlineMaps: true
    property var job: null
    property var mmpk: null

    MapView {
        id: mapView
        anchors.fill: parent

        ProgressBar {
            id: progressBar_loading
            anchors.centerIn: parent
            visible: (value === 0.0 || value === 1.0) ? false : true
        }

        Map {
            id: arcgisOnlineMap
            item: portalItem
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
            onlineMap: arcgisOnlineMap

            onErrorChanged: console.log(error.message, error.additionalMessage);

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                // fetch preplanned map areas from the portal item
                offlineMapTask.preplannedMapAreas();
                path = outputMapPackage;
                fileFolder.url = path;
                const worked = fileFolder.makeFolder();
                if (!worked)
                    console.log("Make Folder Failed");
            }

            onPreplannedMapAreasStatusChanged: {
                if (preplannedMapAreasStatus !== Enums.TaskStatusCompleted)
                    return;

                preplannedCombo.model = offlineMapTask.preplannedMapAreaList;
                busy.visible = false;

                for (let i = 0; i < offlineMapTask.preplannedMapAreaList.count; i++) {
                    const mapArea = offlineMapTask.preplannedMapAreaList.get(i);
                    mapArea.loadStatusChanged.connect(()=> {
                        if (offlineMapTask.preplannedMapAreaList.get(i).loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        const graphic = ArcGISRuntimeEnvironment.createObject("Graphic", { geometry: offlineMapTask.preplannedMapAreaList.get(i).areaOfInterest });

                        graphicsOverlay.graphics.append(graphic);
                    });
                    offlineMapTask.preplannedMapAreaList.get(i).load();
                }
            }

            onCreateDefaultDownloadPreplannedOfflineMapParametersStatusChanged: {
                if (createDefaultDownloadPreplannedOfflineMapParametersStatus !== Enums.TaskStatusCompleted)
                    return;

                /* Set the update mode to not receive updates.
                 * Other options:
                 * SyncWithFeatureServices - changes will be synced directly with the
                 * underlying feature services.
                 * DownloadScheduledUpdates - schedulded, read-only updates will be
                 * downloaded and applied to the local geodatabase. */
                createDefaultDownloadPreplannedOfflineMapParametersResult.updateMode = Enums.PreplannedUpdateModeNoUpdates;
                const result = createDefaultDownloadPreplannedOfflineMapParametersResult;

                path = outputMapPackage +"/" + result.preplannedMapArea.portalItem.title;
                fileFolder.url = path;

                if (fileFolder.exists) {
                    mmpk = ArcGISRuntimeEnvironment.createObject("MobileMapPackage", { path: path });
                    mmpk.loadStatusChanged.connect(()=> {
                        if (loadStatus !== Enums.LoadStatusLoaded )
                            return;

                        if (mmpk.maps.length < 1)
                            return;

                        mapView.map = mmpk.maps[0];
                        busy.visible = false;
                    });
                    mmpk.load();
                } else {
                    job = offlineMapTask.downloadPreplannedOfflineMapWithParameters(createDefaultDownloadPreplannedOfflineMapParametersResult, path);

                    job.jobStatusChanged.connect(()=> {
                        if (job.jobStatus === Enums.JobStatusFailed) {
                            console.log(job.error.message + " - " + job.error.additionalMessage)
                            busy = false;
                            return;
                        } else if (job.jobStatus !== Enums.JobStatusSucceeded) {
                            return;
                        }

                        mapView.map = job.result.offlineMap;
                        mapExists = true;
                        busy.visible = false;
                    });

                    job.progressChanged.connect(()=> {
                        progressBar_loading.value = job.progress * .01;
                    });

                    // Start job to take preplanned map area offline.
                    job.start();
                    busy.visible = false;
                }
            }
        }
    }

    PortalItem {
        id: portalItem
        itemId: "acc027394bc84c2fb04d1ed317aac674"

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            busy.visible = true;

            // Load offline map task.
            offlineMapTask.load();
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
            width: 250
            Button {
                id: onlineMapButton
                Layout.fillWidth: true
                Layout.margins: 1
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Show Online Map")
                enabled: !busy.visible & !viewingOnlineMaps & !progressBar_loading.visible
                onClicked: {
                    mapView.map = arcgisOnlineMap;
                    graphicsOverlay.visible = true;
                    viewingOnlineMaps = true;
                    mapView.setViewpointGeometry(offlineMapTask.preplannedMapAreaList.get(preplannedCombo.currentIndex).areaOfInterest);
                    checkIfFileExists(preplannedCombo.currentIndex);
                }
            }

            Text {
                text: qsTr("Available Preplanned Areas:")
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 1
            }

            ComboBox {
                id: preplannedCombo
                Layout.fillWidth: true
                Layout.margins: 1
                enabled: !busy.visible && !progressBar_loading.visible
                model: null
                textRole: "itemTitle"

                onActivated: {
                    if (offlineMapTask.preplannedMapAreaList.count <= 0)
                        return;

                    if (offlineMapTask.preplannedMapAreaList.get(currentIndex).loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    checkIfFileExists(currentIndex);

                    if (viewingOnlineMaps)
                        mapView.setViewpointGeometry(offlineMapTask.preplannedMapAreaList.get(currentIndex).areaOfInterest);
                }
            }

            Button {
                id: downloadOrView
                Layout.fillWidth: true
                Layout.margins: 1
                enabled: !busy.visible && !progressBar_loading.visible
                text: mapExists ? qsTr("View Preplanned area") : qsTr("Download Preplanned Area")
                onClicked: {
                    preplannedArea = offlineMapTask.preplannedMapAreaList.get(preplannedCombo.currentIndex);

                    // Create the default download parameters appropriate for taking the area offline.
                    offlineMapTask.createDefaultDownloadPreplannedOfflineMapParameters(preplannedArea);
                    if (graphicsOverlay.visible)
                        graphicsOverlay.visible = false;

                    viewingOnlineMaps = false;
                    busy.visible = true;
                }
            }

            Text {
                text: qsTr("Download(s) deleted on exit")
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
            }
        }
    }

    FileFolder {
        id: fileFolder
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: false
    }

    function checkIfFileExists(index) {
        path = outputMapPackage + "/" + offlineMapTask.preplannedMapAreaList.get(index).portalItem.title;
        fileFolder.url = path;
        mapExists = fileFolder.exists;
    }

    Component.onDestruction: {
        fileFolder.url = outputMapPackage;
        fileFolder.cdUp();
        fileFolder.removeFolder("ArcGIS", true);
    }
}
