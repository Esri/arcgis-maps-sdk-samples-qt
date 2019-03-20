// [WriteFile Name=OrbitCameraAroundObject, Category=Scenes]
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

import Esri.Samples 1.0
import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12

Item {

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    OrbitCameraAroundObjectSample {
        id: model
        sceneView: view
        planePitch: planePitchSlider.value
    }

    //Color for the panels that the UI controls sit on.
    property color uiBackgroundCol : Qt.rgba(0.2, 0.2, 0.2, 0.65)

    //Camera heading slider, sits at the bottom of the screen
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 30
        }

        height: cameraHeadingSlider.height + cameraHeadingLabel.height
        color: uiBackgroundCol

        Slider {
            id: cameraHeadingSlider

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            value: model.cameraHeading //Value bound to the model, so rotating the camera will update the slider.
            from: model.cameraHeadingBounds.x //Similarly, setting different initial bounds changes the UI automatically. Type used is QPointF for min/max, hence the x/y
            to: model.cameraHeadingBounds.y


            //To avoid getting stuck in a binding loop for the value, update the value of the camera heading from the slider only in response to a moving slider.
            onMoved: {
                model.cameraHeading = cameraHeadingSlider.value
            }

            //Custom slider handle that displays the current value
            handle: Rectangle {
                x: cameraHeadingSlider.leftPadding + cameraHeadingSlider.visualPosition * (cameraHeadingSlider.availableWidth - width)
                y: cameraHeadingSlider.topPadding + cameraHeadingSlider.availableHeight / 2 - height / 2
                implicitWidth: 30
                implicitHeight: 30

                Text {
                    id: headingValue

                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }

                    text: Math.round(cameraHeadingSlider.value)
                    color: "black"
                }
            }
        }

        Text {
            id: cameraHeadingLabel

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: cameraHeadingSlider.top
            }

            text: "Camera Heading"
            color: "white"
        }
    }

    //Plane pitch slider, placed in the top-right of the screen
    Rectangle {
        anchors {
            top: parent.top
            bottom: parent.verticalCenter
            right: parent.right
            margins: 30
        }

        width: childrenRect.width
        color: uiBackgroundCol

        ColumnLayout {
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            spacing: 5

            Text {
                id: planePitchLabel
                text: "Plane Pitch"
                color: "white"

            }

            Slider {
                id: planePitchSlider

                Layout.fillHeight: true

                value: 0
                from: 90
                to: -90
                orientation: Qt.Vertical

                //Custom slider handle that displays the current value
                handle: Rectangle {
                    x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - width / 2
                    y: planePitchSlider.topPadding + planePitchSlider.visualPosition * (planePitchSlider.availableHeight - height)

                    implicitWidth: 30
                    implicitHeight: 30

                    Text {
                        id: pitchValue

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                        }

                        text: Math.round(planePitchSlider.value)
                        color: "black"
                    }
                }
            }
        }
    }

    //View change buttons / allow cam interaction checkbox placed in the top-left of the screen.
    Rectangle{

        anchors {
            left: parent.left
            top: parent.top
        }

        height: childrenRect.height
        width: childrenRect.width //Add a little bit of manual padding to comfortably fit the checkbox text
        color: uiBackgroundCol

        Column {
            spacing: 10
            padding: 10
            Button {
                text: "Cockpit View"
                onClicked: model.cockpitView()
            }

            Button {
                text: "Center View"
                onClicked: model.centerView()
            }

            Row {
                anchors {
                    left: parent.left
                }

                CheckBox {
                    id: allowCamDistanceInteractionCheckBox
                    checked: model.allowCamDistanceInteraction
                    onCheckedChanged: model.allowCamDistanceInteraction = checked
                }

                Text {
                    anchors {
                        verticalCenter: allowCamDistanceInteractionCheckBox.verticalCenter
                    }
                    text: "Allow camera\ndistance interaction"
                    color: "white"
                }
            }
        }
    }
}
