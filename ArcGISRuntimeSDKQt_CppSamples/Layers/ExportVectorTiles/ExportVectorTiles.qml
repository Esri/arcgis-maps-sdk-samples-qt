// [WriteFile Name=ExportVectorTiles, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {
    id: item

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        id: exportProgressWindow
        anchors.centerIn: parent

        color: "white"
        visible: model.jobStatus !== 0 && model.jobStatus !== 3 && model.jobStatus !== 4

        border {
            color: "black"
            width: 2
        }

        Column {
            id: exportProgressColumn
            anchors.centerIn: parent
            spacing: 10

            BusyIndicator {
                id: busyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: statusText
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Export job status: " + ["Not started", "Started", "Paused", "Succeeded", "Failed", "Cancelling"][model.jobStatus]
                font.pixelSize: 16
            }

            Text {
                id: statusTextCanceled
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Job cancelled"
                visible: !statusText.visible
                font.pixelSize: 16
            }

            Text {
                id: progressText
                anchors.horizontalCenter: parent.horizontalCenter
                text: model.exportProgress + "% Completed"
                font.pixelSize: 16
            }

            onWidthChanged: {
                exportProgressWindow.width = exportProgressColumn.width + 20
            }

            onHeightChanged: {
                exportProgressWindow.height = exportProgressColumn.height + 20
            }
        }
    }

    // Create an extent rectangle for selecting the offline area
    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent

        width: parent.width - parent.width * 0.1
        height: parent.height - parent.height * 0.25
        color: "transparent"
        border {
            color: "red"
            width: 3
        }
    }

    // Button to start the download
    Button {
        id: button
        anchors {
            bottom: parent.bottom
            bottomMargin: item.height * .05
            horizontalCenter: parent.horizontalCenter
        }
        width: 150

        text: "Export area"

        onClicked: {
            switch(model.jobStatus) {
            case ExportVectorTilesSample.ExportStatusNotStarted:
                model.startExport(extentRectangle.x, (extentRectangle.y + extentRectangle.height), (extentRectangle.x + extentRectangle.width), extentRectangle.y);
                extentRectangle.visible = false;
                break;
            case ExportVectorTilesSample.ExportStatusStarted:
                model.cancel();
                extentRectangle.visible = true;
                break;
            case ExportVectorTilesSample.ExportStatusPaused:
                break;
            case ExportVectorTilesSample.ExportStatusSucceeded:
                model.reset();
                extentRectangle.visible = true;
                break;
            case ExportVectorTilesSample.ExportStatusFailed:
                model.startExport(extentRectangle.x, (extentRectangle.y + extentRectangle.height), (extentRectangle.x + extentRectangle.width), extentRectangle.y);
                break;
            case ExportVectorTilesSample.ExportStatusCancelling:
                break;
            default:
                break;
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ExportVectorTilesSample {
        id: model
        mapView: view

        onJobStatusChanged: {
            switch(model.jobStatus) {
            case ExportVectorTilesSample.ExportStatusNotStarted:
                button.text = "Export area";
                break;
            case ExportVectorTilesSample.ExportStatusStarted:
                button.text = "Cancel export";
                exportProgressWindow.visible = true;
                break;
            case ExportVectorTilesSample.ExportStatusPaused:
                break;
            case ExportVectorTilesSample.ExportStatusSucceeded:
                button.text = "Reset";
                exportProgressWindow.visible = false;
                break;
            case ExportVectorTilesSample.ExportStatusFailed:
                button.text = "Export area";
                exportProgressWindow.visible = false;
                break;
            case ExportVectorTilesSample.ExportStatusCancelling:
                break;
            default:
                break;
            }
        }

        readonly property Timer timer: Timer {
            id: jobCancelDoneTimer
            interval: 2000
            onTriggered: { exportProgressWindow.visible = false; statusText.visible = true }
        }

        onJobCancelDone: succeeded => {
                             statusTextCanceled.text = "Job canceled " + (succeeded ? "successfully" : "not successfully")
                             exportProgressWindow.visible = true;
                             statusText.visible = false;
                             jobCancelDoneTimer.start();
                         }
    }
}
