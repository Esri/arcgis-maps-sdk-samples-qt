// [WriteFile Name=EditAndSyncFeatures, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import Esri.Samples 1.0

EditAndSyncFeaturesSample {
    id: editAndSyncSample
    clip: true
    width: 800
    height: 600
    
    property string statusText: ""
    property string instructionText: ""
    property alias isOffline: editAndSyncSample.isOffline
    property var selectedFeature: null

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    onHideWindow: {
        syncWindow.hideWindow(time);

        if (success) {
            extentRectangle.visible = false;
            syncButton.visible = false;
        }
    }

    onUpdateInstruction: {
        instructions.visible = true;
        instructionText = instruction;
    }

    onShowButton: syncButton.visible = true;

    onUpdateStatus: statusText = status;

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
    Rectangle {
        id: syncButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }

        width: isOffline ? 175 : 200
        height: 35
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 8
        border {
            color: "#585858"
            width: 1
        }

        Row {
            anchors.fill: parent
            spacing: 5
            Image {
                width: 38
                height: width
                source: isOffline ? "qrc:/Samples/EditData/EditAndSyncFeatures/sync.png" : "qrc:/Samples/EditData/EditAndSyncFeatures/download.png"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: isOffline ? "Sync Geodatabase" : "Generate Geodatabase"
                font.pixelSize: 14
                color: "#474747"
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: syncButton.pressed = true
            onReleased: syncButton.pressed = false
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
        color: "gray"
        opacity: 0.9
        visible: false

        Text {
            anchors.centerIn: parent
            text: instructionText
            font.pixelSize: 16
            color: "white"
        }
    }

    // Create a window to display the generate/sync window
    Rectangle {
        id: syncWindow
        anchors.fill: parent
        color: "transparent"
        clip: true
        visible: false

        RadialGradient {
            anchors.fill: parent
            opacity: 0.7
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 125
            height: 100
            color: "lightgrey"
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

                Text {
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
