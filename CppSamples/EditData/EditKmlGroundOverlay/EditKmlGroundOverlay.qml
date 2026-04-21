// [WriteFile Name=EditKmlGroundOverlay, Category=EditData]
// [Legal]
// Copyright 2019 Esri.
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

Item {

    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Slider UI presentation at bottom
        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: view.attributionTop
            }
            width: childrenRect.width
            height: childrenRect.height
            radius: 5
            color: palette.base
            border {
                width: 1
                color: "darkgrey"
            }

            Column {
                padding: 8
                spacing: 5

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: "Opacity"
                }

                // sliderCombo: A slider and text for its value
                Row {
                    id: sliderCombo

                    Slider {
                        id: slider
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }
                        // Slider controls degrees of rotation
                        from: 0
                        to: 255
                        value: 255
                        stepSize: 25
                        onValueChanged: {
                            // modify the overlay's color/alpha value
                            model.setOpacity(value);
                        }
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    EditKmlGroundOverlaySample {
        id: model
        sceneView: view
    }
}
