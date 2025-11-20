// [WriteFile Name=ScenePropertiesExpressions, Category=Scenes]
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
import QtQuick.Layouts
import Esri.Samples

Item {
    id: root
    width: 800
    height: 800

    SceneView {
        id: sceneView
        anchors.fill: parent
        objectName: "sceneView"

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        width: sliderColumn.implicitWidth + 16
        height: sliderColumn.implicitHeight + 16
        color: palette.base
        radius: 5

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        border {
            width: 1
            color: "darkgrey"
        }

        Column {
            id: sliderColumn
            spacing: 2

            anchors {
                left: parent.left
                top: parent.top
                centerIn: parent
            }

            height: childrenRect.height

            Label {
                text: qsTr("Pitch : " + model.pitch.toFixed(0))
                verticalAlignment: Text.AlignTop
            }

            Slider {
                id: pitchSlider
                height: 64

                // slider controls degrees of rotation:
                from: -90
                to: 90
                value: 90
            }

            Label {
                text: qsTr("Heading: " + model.heading.toFixed(0))
                verticalAlignment: Text.AlignTop
            }

            Slider {
                id: headingSlider
                height: 64

                // slider controls degrees of rotation:
                from: 0
                to: 360
                value: 180
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
