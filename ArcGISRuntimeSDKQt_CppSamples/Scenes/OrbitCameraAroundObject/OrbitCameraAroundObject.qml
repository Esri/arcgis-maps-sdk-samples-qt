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

    //Camera heading slider, sits at the bottom of the screen
    Slider {
        id: cameraHeadingSlider

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 20
        }

        value: model.cameraHeading //Value bound to the model, so rotating the camera will update the slider.
        from: model.cameraHeadingBounds.x //Similarly, setting different initial bounds changes the UI automatically. Type used is QPointF for min/max, hence the x/y
        to: model.cameraHeadingBounds.y

        //To avoid getting stuck in a binding loop for the value, update the value of the camera heading from the slider only in response to a moving slider.
        onMoved: {
            model.cameraHeading = cameraHeadingSlider.value
        }

        //Custom slider handle that displays the current value
        handle: Item {
            x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - headingHandleNub.width)
            y: parent.topPadding + parent.availableHeight / 2 - headingHandleNub.height / 2


            Rectangle {
                id: headingHandleNub
                color: headingHandleRect.color
                radius: width * 0.5
                width: 10
                height: width
            }
            Rectangle {
                id: headingHandleRect
                height: childrenRect.height
                width: childrenRect.width
                radius: 3
                x: headingHandleNub.x - width / 2 + headingHandleNub.width / 2
                y: headingHandleNub.y - height
                color: cameraHeadingSlider.background.children[0].color

                Text {
                    id: headingValue
                    font.pixelSize: 14
                    padding: 3
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    text: Math.round(cameraHeadingSlider.value) + "\u00B0"
                    color: "white"
                }
            }
        }
    }

    Text {
        id: cameraHeadingLabel

        anchors {
            horizontalCenter: cameraHeadingSlider.horizontalCenter
            bottom: cameraHeadingSlider.top
            bottomMargin: 10
        }

        text: "Camera Heading"
        color: "white"
    }

    //Plane pitch slider, placed in the top-right of the screen
    Text {
        id: planePitchLabel

        anchors {
            horizontalCenter: planePitchSlider.horizontalCenter
            bottom: planePitchSlider.top
            bottomMargin: 10
        }

        text: "Plane Pitch"
        color: "white"
    }

    Slider {
        id: planePitchSlider

        anchors {
            top: parent.top
            bottom: parent.verticalCenter
            right: parent.right
            margins: 30
        }

        value: 0
        from: 90
        to: -90
        orientation: Qt.Vertical

        //Custom slider handle that displays the current value
        handle: Item {
            x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - pitchHandleNub.width / 2
            y: planePitchSlider.topPadding + planePitchSlider.visualPosition * (planePitchSlider.availableHeight - pitchHandleNub.height)

            Rectangle {
                id: pitchHandleNub
                color: pitchHandleRect.color
                radius: width * 0.5
                width: 10
                height: width
            }
            Rectangle {
                id: pitchHandleRect
                height: childrenRect.height
                width: childrenRect.width
                radius: 3
                x: pitchHandleNub.x - width
                y: pitchHandleNub.y - height/2 + pitchHandleNub.height/2
                color: planePitchSlider.background.children[0].color

                Text {
                    id: pitchValue
                    font.pixelSize: 14
                    padding: 3
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    text: Math.round(planePitchSlider.value)  + "\u00B0"
                    color: "white"
                }
            }
        }
    }

    //View change buttons / allow cam interaction checkbox placed in the top-left of the screen.
    Rectangle {
        anchors {
            left: parent.left
            top: parent.top
        }

        height: childrenRect.height
        width: childrenRect.width
        color: Qt.rgba(0.2, 0.2, 0.2, 0.65)

        Column {
            spacing: 10
            padding: 10
            Button {
                text: "Cockpit View"
                onClicked: {
                    model.cockpitView();
                    allowCamDistanceInteractionCheckBox.enabled = false;
                    allowCamDistanceInteractionCheckBoxText.color = "gray";
                }
            }

            Button {
                text: "Center View"
                onClicked: {
                    model.centerView();
                    allowCamDistanceInteractionCheckBox.enabled = true;
                    allowCamDistanceInteractionCheckBoxText.color = "white";
                }
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
                    id: allowCamDistanceInteractionCheckBoxText
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
