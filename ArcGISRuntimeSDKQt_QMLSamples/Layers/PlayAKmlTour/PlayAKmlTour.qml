// [WriteFile Name=PlayAKmlTour, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.6

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }
        }

        Rectangle {
            id: buttonBackground
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#000000"
            opacity: .75
            radius: 5

            RowLayout {
                spacing: 0
                Button {
                    id: playButton
                    text: qsTr("Play")
                    Layout.margins: 2
                    visible: !pauseButton.visible
                    enabled: false
                }

                Button {
                    id: pauseButton
                    text: qsTr("Pause")
                    Layout.margins: 2
                    visible: !playButton.visible
                    enabled: false
                }

                Button {
                    id: resetButton
                    text: qsTr("Reset")
                    Layout.margins: 2
//                    enabled: false
                    onClicked: playButton.visible ? playButton.visible = false : playButton.visible = true
                }
            }

        }
    }
}
