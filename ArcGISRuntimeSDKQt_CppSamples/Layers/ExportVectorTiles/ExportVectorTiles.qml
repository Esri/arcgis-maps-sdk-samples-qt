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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import Esri.Samples 1.0
import QtGraphicalEffects 1.0

Item {
    id: item1

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
        visible: false

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
                text: "Export in progress..."
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
        visible: button.enabled
    }

    Button {
        id: button
        width: buttonImage.width + buttonText.width + (buttonTextRow.spacing * 2)

        Row {
            id: buttonTextRow
            anchors.fill: parent
            layoutDirection: Qt.LeftToRight
            spacing: 5
            Image {
                id: buttonImage
                width: button.height
                height: button.height
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/Samples/Layers/ExportVectorTiles/download.png"
            }

            Text {
                id: buttonText
                text: "Export area"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        anchors {
            bottom: parent.bottom
            bottomMargin: item1.height * .05
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            model.startExport(extentRectangle.x, (extentRectangle.y + extentRectangle.height), (extentRectangle.x + extentRectangle.width), extentRectangle.y);
            extentRectangle.visible = false;
            button.visible = false;
            exportProgressWindow.visible = true
            statusText.text = "Initializing export task";
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ExportVectorTilesSample {
        id: model
        mapView: view
        property bool exportInProgress: false

        onExportProgressChanged: {
            if (!exportInProgress && exportProgress !== 100) {
                statusText.text = "Exporting vector tiles";
                exportInProgress = true;
            } else if (exportProgress === 100) {
                exportProgressWindow.visible = false
            }
        }
    }
}
