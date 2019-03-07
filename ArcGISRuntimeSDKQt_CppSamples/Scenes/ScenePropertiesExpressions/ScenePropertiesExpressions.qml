// [WriteFile Name=ScenePropertiesExpressions, Category=Scenes]
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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import Esri.Samples 1.0

Item {
    id: root
    width: 800
    height: 800

    SceneView {
        id: sceneView
        anchors.fill: parent
        objectName: "sceneView"
    }

    Rectangle {
        anchors.fill: sliderColumn
        color: "lightblue"
    }

    Column{
        id: sliderColumn

        spacing: 4

        anchors {
            left: parent.left
            top: parent.top
        }

        height: childrenRect.height

        Text {
            anchors {
                margins: 5
            }
            text: "pitch : " + model.pitch.toFixed(0)
            font.pixelSize: 20
            verticalAlignment: Text.AlignTop
        }

        Slider{
            id: pitchSlider
            opacity: 0.7
            height: 64

            // slider controls degrees of rotation:
            from: -90
            to: 90
            value: 90
            anchors {
                margins: 5
            }
        }

        Text {
            anchors {
                margins: 5
            }
            text: "heading: " + model.heading.toFixed(0)
            verticalAlignment: Text.AlignTop
            font.pixelSize: 20
        }

        Slider{
            id: headingSlider
            opacity: 0.7
            height: 64

            // slider controls degrees of rotation:
            from: 0
            to: 360
            value: 180
            anchors {
                margins: 5
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ScenePropertiesExpressionsSample {
        id: model
        sceneView: sceneView
        pitch: pitchSlider.value
        heading: headingSlider.value
    }
}
