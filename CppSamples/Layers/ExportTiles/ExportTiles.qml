// [WriteFile Name=ExportTiles, Category=Layers]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import Esri.Samples

ExportTilesSample {
    id: exportTilesSample
    width: 800
    height: 600

    property string statusText: ""

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Create the download button to export tile cache
        Button {
            id: exportTilesButton
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 5
            }

            text: qsTr("Export tiles")
            leftPadding: 20
            rightPadding: 20
            icon {
                source: "qrc:/Samples/Layers/ExportTiles/download-24.svg"
                width: 24
                height: 24
                color: palette.buttonText
            }

            onClicked: {
                // call the C++ invokable function to export tile cache from the input screen coordinates
                exportTilesSample.exportTileCacheFromCorners(extentRectangle.x, extentRectangle.y, (extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
                exportWindow.visible = true;
            }
        }
    }

    onHideWindow: (time, success) => {
                      exportWindow.hideWindow(time);

                      if (success) {
                          extentRectangle.visible = false;
                          downloadButton.visible = false;
                      }
                  }

    onUpdateStatus: status => statusText = status;

    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - 50
        height: parent.height - 125
        color: "transparent"
        border {
            color: "red"
            width: 3
        }
    }

    // Create a window to display the export window
    Rectangle {
        id: exportWindow
        anchors.fill: parent
        color: "transparent"
        visible: false
        clip: true

        Rectangle {
            anchors.fill: parent
            color: "#60000000"
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 140
            height: 145
            color: palette.base
            opacity: 0.8
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10
                }
                spacing: 10

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr(statusText)
                    font.pixelSize: 16
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr(exportTilesSample.exportTilesProgress + "% Completed")
                    font.pixelSize: 16
                }
            }
        }

        Timer {
            id: hideWindowTimer

            onTriggered: exportWindow.visible = false;
        }

        function hideWindow(time) {
            hideWindowTimer.interval = time;
            hideWindowTimer.restart();
        }
    }
}
