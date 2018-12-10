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
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    states: [
        State {
            when: noAtmosphereButton.checked
            PropertyChanges {
                target: sceneView
                atmosphereEffect: Enums.AtmosphereEffectNone
            }
        },
        State {
            when: realisticAtmosphereButton.checked
            PropertyChanges {
                target: sceneView
                atmosphereEffect: Enums.AtmosphereEffectRealistic
            }
        },
        State {
            when: horizonAtmosphereButton.checked
            PropertyChanges {
                target: sceneView
                atmosphereEffect: Enums.AtmosphereEffectHorizonOnly
            }
        }
    ]

    SceneView {
        id: sceneView
        anchors.fill: parent

        Component.onCompleted: {
            setViewpointCameraAndSeconds(camera, 0);
        }

        Camera {
            id: camera
            Point {
                y: 64.416919
                x: -14.483728
                z: 100
            }
            heading : 318
            pitch : 105
            roll: 0
        }

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }
        }
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
                text: "Horizon only"
                checkable: true
                checked: true
            }
        }

        ButtonGroup {
            buttons: [noAtmosphereButton, realisticAtmosphereButton, horizonAtmosphereButton]
        }
    }
}
