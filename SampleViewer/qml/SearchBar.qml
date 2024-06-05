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

TextField {
    id: searchBar
    rightPadding: height
    inputMethodHints: Qt.ImhNoPredictiveText
    selectByMouse: true

    Image {
        id: clearButton
        anchors {
            top: parent.top
            right: parent.right
        }
        visible : opacity !== 0
        source: "qrc:/ic_menu_closeclear_light.png"
        fillMode: Image.PreserveAspectFit
        height: parent.height - 10

        states: [
            State {
                when: searchBar.length === 0
                PropertyChanges {
                    target: clearButton
                    opacity: 0.0
                }
            },
            State {
                when: searchBar.length > 0
                PropertyChanges {
                    target: clearButton
                    opacity: 1.0
                }
            }
        ]

        transitions: Transition {
            NumberAnimation {
                property: "opacity"
                duration: 200
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            preventStealing: true
            onPressed: {
                searchBar.text = "";
            }
        }
    }
}
