// [WriteFile Name=ChangeAtmosphereEffect, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {
    states: [
        State {
            when: noAtmosphereButton.checked
            PropertyChanges {
                target: sample
                atmosphereEffect: ChangeAtmosphereEffectSample.None
            }
        },
        State {
            when: realisticAtmosphereButton.checked
            PropertyChanges {
                target: sample
                atmosphereEffect: ChangeAtmosphereEffectSample.Realistic
            }
        },
        State {
            when: horizonAtmosphereButton.checked
            PropertyChanges {
                target: sample
                atmosphereEffect: ChangeAtmosphereEffectSample.HorizonOnly
            }
        }
    ]

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ChangeAtmosphereEffectSample {
        id: sample
        sceneView: view
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }

        color: Qt.rgba(1, 1, 1, 0.3);
        width: childrenRect.width
        height: childrenRect.height
        ColumnLayout {
            id: layout

            Text {
                Layout.margins: 5
                color: "white"
                text: "Atmosphere effects"
            }

            Button {
                id: noAtmosphereButton
                Layout.margins: 5
                Layout.fillWidth: true
                text: "None"
                checkable: true
            }

            Button {
                id: realisticAtmosphereButton
                Layout.margins: 5
                Layout.fillWidth: true
                text: "Realistic"
                checkable: true
            }

            Button {
                id: horizonAtmosphereButton
                Layout.margins: 5
                Layout.fillWidth: true
                text: "Horizon Only"
                checkable: true
                checked: true
            }
        }

        ButtonGroup {
            buttons: [noAtmosphereButton, realisticAtmosphereButton, horizonAtmosphereButton]
        }
    }
}
