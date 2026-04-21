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
import Calcite

TextField {
    id: searchBar
    rightPadding: height
    inputMethodHints: Qt.ImhNoPredictiveText
    selectByMouse: true

    ToolButton {
        id: clearButton
        anchors {
            top: parent.top
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        visible: opacity !== 0
        icon.source: "qrc:/x.svg"
        icon.width: parent.height - 10
        icon.height: parent.height - 10
        icon.color: Calcite.text1
        flat: true
        background: Item {}

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

        onClicked: searchBar.text = ""
    }
}
