// [WriteFile Name=ApplyScheduledMapUpdates, Category=Maps]
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
import QtQuick.Controls 2.4
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string tempDataPath: System.temporaryFolder.path + "/" + new Date().getTime()
    readonly property string origMmpkPath: System.userHomePath + "/ArcGIS/Runtime/Data/mmpk/canyonlands/canyonlands.zip"
    readonly property string destMmpkPath: tempDataPath + "/canyonlands.zip"
    property MobileMapPackage mmpk

    // For the purposes of demonstrating the sample,
    // create a temporary copy of the local offline map files,
    // so that updating does not overwrite them permanently
    Component.onCompleted: {
        // copy the zip
        fileFolder.makePath(tempDataPath)
        fileFolder.copyFile(origMmpkPath, destMmpkPath);

        // unzip the archive
        zipArchive.extractAll(tempDataPath + "/canyonlands")
    }

    function createMmpk() {
        mmpk = ArcGISRuntimeEnvironment.createObject("MobileMapPackage", {
                                                         path: System.resolvedPathUrl(tempDataPath + "/canyonlands")
                                                     });
        mmpk.loadStatusChanged.connect(()=> {
                                           if (mmpk.loadStatus === Enums.LoadStatusLoaded) {
                                               // check size of maps
                                               if (mmpk.maps.length === 0)
                                                   return;

                                               // get map from mmpk
                                               const map = mmpk.maps[0];

                                               // set on the mapview for display
                                               mapView.map = map;

                                               // hook up to offline map task
                                               offlineMapSyncTask.map = map;

                                               // check for updates
                                               offlineMapSyncTask.checkForUpdates();
                                           } else if (mmpk.loadStatus === Enums.LoadStatusFailedToLoad) {
                                               console.log("failed to load", mmpk.error.message, mmpk.error.additionalMessage);
                                           }
                                       });

        mmpk.load();
    }

    MapView {
        id: mapView
        anchors.fill: parent
    }

    OfflineMapSyncTask {
        id: offlineMapSyncTask

        property var syncJob

        onCheckForUpdatesStatusChanged: {
            if (checkForUpdatesStatus === Enums.TaskStatusInProgress) {
                console.log("Check for updates in progress...");
            } else if (checkForUpdatesStatus === Enums.TaskStatusErrored) {
                console.log("Check for updates error:", error.message, error.additionalMessage);
            } else if (checkForUpdatesStatus === Enums.TaskStatusCompleted) {
                console.log("Check for updates complete");

                // if updates are available, enable the download button & set detail text
                if (checkForUpdatesResult.downloadAvailability === Enums.OfflineUpdateAvailabilityAvailable) {
                    // enable the button
                    downloadUpdatesButton.enabled = true;

                    // set the text
                    availabilityText.text = "Updates Available";
                    availabilityDetailsText.text = "Updates size: %1 bytes".arg(offlineMapSyncTask.checkForUpdatesResult.scheduledUpdatesDownloadSize);
                } else {
                    // disable the button
                    downloadUpdatesButton.enabled = false;

                    // set the text
                    availabilityText.text = "No updates available";
                    availabilityDetailsText.text = "The preplanned map area is up to date"
                }

                busyIndicator.visible = false;
            }
        }

        onCreateDefaultOfflineMapSyncParametersStatusChanged: {
            if (createDefaultOfflineMapSyncParametersStatus === Enums.TaskStatusInProgress) {
                console.log("Create Parameters in progress...");
            } else if (createDefaultOfflineMapSyncParametersStatus === Enums.TaskStatusErrored) {
                console.log("Create Parameters error:", error.message, error.additionalMessage);
            } else if (createDefaultOfflineMapSyncParametersStatus === Enums.TaskStatusCompleted) {
                console.log("Create Parameters complete");

                // get the parameters result
                const params = createDefaultOfflineMapSyncParametersResult;

                // set the parameters to download all updates for the mobile map packages
                params.preplannedScheduledUpdatesOption = Enums.PreplannedScheduledUpdatesOptionDownloadAllUpdates;

                // create the job
                syncJob = offlineMapSyncTask.syncOfflineMap(createDefaultOfflineMapSyncParametersResult);

                // connect to the job signals
                syncJob.jobStatusChanged.connect(()=> {
                                                     if (syncJob.jobStatus === Enums.JobStatusSucceeded) {
                                                         const mapResult = syncJob.result;

                                                         if (mapResult.mobileMapPackageReopenRequired) {
                                                             // close and reopen the MMPK if required
                                                             mmpk.close();
                                                             createMmpk();
                                                         } else {
                                                             console.log("not required");
                                                         }
                                                         busyIndicator.visible = false;

                                                         // re-check if updates are available
                                                         offlineMapSyncTask.checkForUpdates();
                                                     } else if (syncJob.jobStatus === Enums.JobStatusFailed) {
                                                         console.log("sync job failed");
                                                         busyIndicator.visible = false;
                                                     }
                                                 });

                // start the job
                syncJob.start();
            }
        }
    }

    Rectangle {
        anchors.fill: controlColumn
        anchors.margins: -5
        color: "#404040"
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Button {
            id: downloadUpdatesButton
            text: "Apply Scheduled Updates"
            enabled: false

            onClicked: {
                busyIndicator.visible = true;
                offlineMapSyncTask.createDefaultOfflineMapSyncParameters();
            }
        }

        Text {
            id: availabilityText
            color: "white"
        }

        Text {
            id: availabilityDetailsText
            color: "white"
        }
    }

    ZipArchive {
        id: zipArchive
        path: destMmpkPath

        onExtractCompleted: {
            // create the mmpk once the zip archive is copied
            createMmpk();
        }
    }

    FileFolder {
        id: fileFolder
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
    }
}
