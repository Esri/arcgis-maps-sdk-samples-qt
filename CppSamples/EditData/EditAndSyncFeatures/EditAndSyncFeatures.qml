// [WriteFile Name=EditAndSyncFeatures, Category=EditData]
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

EditAndSyncFeaturesSample {
    id: editAndSyncSample
    clip: true
    width: 800
    height: 600

    property string statusText: ""
    property string instructionText: ""
    property var selectedFeature: null

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    onHideWindow: (time, success) => {
                      syncWindow.hideWindow(time);

                      if (success) {
                          extentRectangle.visible = false;
                          syncButton.visible = false;
                      }
                  }

    onUpdateInstruction: instruction => {
                             instructions.visible = true;
                             instructionText = instruction;
                         }

    onShowButton: syncButton.visible = true;

    onUpdateStatus: status => statusText = status;

    // create an extent rectangle for the output geodatabase
    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - (50)
        height: parent.height - (125)
        color: "transparent"
        border {
            color: "red"
            width: 3
        }
    }

    // Create the button to generate/sync geodatabase
    Button {
        id: syncButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }

        text: isOffline ? qsTr("Sync Geodatabase") : qsTr("Generate Geodatabase")
        leftPadding: 20
        rightPadding: 20
        icon {
            source: isOffline ? "qrc:/Samples/EditData/EditAndSyncFeatures/sync.png" : "qrc:/Samples/EditData/EditAndSyncFeatures/download.png"
            width: 24
            height: 24
            color: palette.text
        }

        onClicked: {
            if (isOffline) {
                instructions.visible = false;
                editAndSyncSample.executeSync();
                syncWindow.visible = true;
            } else {
                editAndSyncSample.generateGeodatabaseFromCorners(extentRectangle.x, extentRectangle.y, (extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
                syncWindow.visible = true;
            }
        }
    }

    // Create a bar to display editing instructions
    Rectangle {
        id: instructions
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 25
        color: palette.base
        opacity: 0.9
        visible: false

        Label {
            anchors.centerIn: parent
            text: instructionText
            font.pixelSize: 16
        }
    }

    // Create a window to display the generate/sync window
    Rectangle {
        id: syncWindow
        anchors.fill: parent
        color: "transparent"
        clip: true
        visible: false

        Rectangle {
            anchors.fill: parent
            color: "#60000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 125
            height: 100
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
                    text: statusText
                    font.pixelSize: 16
                }
            }
        }

        Timer {
            id: hideWindowTimer

            onTriggered: {
                syncWindow.visible = false;
                instructions.visible = true;
                instructionText = "Tap on a feature";
            }
        }

        function hideWindow(time) {
            hideWindowTimer.interval = time;
            hideWindowTimer.restart();
        }
    }
}
