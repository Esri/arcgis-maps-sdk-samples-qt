// [WriteFile Name=RealisticLightingAndShadows, Category=Scenes]
// [Legal]
// Copyright 2020 Esri.

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
import Esri.Samples

Item {

    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    RealisticLightingAndShadowsSample {
        id: model
        sceneView: view
        onSunTimeChanged: timeValue.text = "August 10, 2018, " + sunTimeText

    }
    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#ffffff"
        opacity: 0.7
        Column {
            id: settingsColumn
            spacing: 5
            padding: 15
            Text {
                id: timeValue
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 16
                // text is set from C++ instance
            }

            ComboBox {
                id: sunLightingSelection
                anchors.horizontalCenter: parent.horizontalCenter
                width: 200
                model: [
                    "No Light",
                    "Light",
                    "Light and Shadows"
                ]
                currentIndex: 2
                onCurrentIndexChanged: {
                    model.setLightingMode(currentIndex);
                }
            }
            Slider {
                id: sunTimeSlider
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.7
                from: 0; to: 23.99
                value: 8.5
                onValueChanged: {
                    model.setSunTimeFromValue(sunTimeSlider.value);
                }
            }
        }
    }
}
