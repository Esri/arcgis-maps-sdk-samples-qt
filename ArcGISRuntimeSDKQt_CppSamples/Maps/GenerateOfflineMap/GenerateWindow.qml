// Copyright 2017 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: exportWindow
    color: "transparent"
    visible: false
    clip: true

    property string statusText: "Starting"
    property string progressText: "0"

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
        width: 135
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
                text: "%1: %2%".arg(statusText).arg(progressText)
                font.pixelSize: 16
            }
        }
    }

    Timer {
        id: hideWindowTimer

        onTriggered: parent.visible = false;
    }

    function hideWindow(time) {
        hideWindowTimer.interval = time;
        hideWindowTimer.restart();
    }
}
