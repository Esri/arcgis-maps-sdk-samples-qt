// [WriteFile Name=TerrainExaggeration, Category=Scenes]
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
import Esri.Samples 1.0

TerrainExaggerationSample {
    id: terrainExaggerationSample
    clip: true
    width: 800
    height: 600
    
    SceneView {
        id: sceneView
        anchors.fill: parent
        objectName: "sceneView"

        // Slider UI presentation at bottom
        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: sceneView.attributionTop
            }

            width: childrenRect.width
            height: childrenRect.height
            radius: 10
            opacity: 0.8

            // sliderCombo: A slider and text for its value
            Row {
                id: sliderCombo
                spacing: 5

                Slider {
                    id: slider
                    opacity: 0.7
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    // Slider controls degrees of rotation
                    from: 1
                    to: 10
                    onValueChanged: {
                        terrainExaggerationSample.setElevationExaggeration(slider.value);
                    }
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        margins: 5
                    }
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 20
                    text: slider.value.toPrecision(2);
                }
            }
        }
    }
}
