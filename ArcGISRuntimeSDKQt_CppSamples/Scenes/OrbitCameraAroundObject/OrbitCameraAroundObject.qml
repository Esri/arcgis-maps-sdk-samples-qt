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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.Samples 1.0

Item {

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    OrbitCameraAroundObjectSample {
        id: model
        sceneView: view
    }

    Rectangle {
        anchors {
            bottom: parent.bottom
            left : parent.left
            right : parent.right
            margins: 30
        }

        height: childrenRect.height //Binding Loop!
        color: Qt.rgba(0.2, 0.2, 0.2, 0.65);

        Slider {
            id: cameraHeadingSlider
            value: 0
            from: -180.0
            to: 180.0

            handle: Rectangle {
                x: cameraHeadingSlider.leftPadding + cameraHeadingSlider.visualPosition * (cameraHeadingSlider.availableWidth - width)
                y: cameraHeadingSlider.topPadding + cameraHeadingSlider.availableHeight / 2 - height / 2
                implicitWidth: 30
                implicitHeight: 30

                Text {
                    id: headingValue
                    text: "0"
                    color: "black"

                    anchors{
                        horizontalCenter : parent.horizontalCenter
                        verticalCenter : parent.verticalCenter
                    }
                }
            }

            anchors {
                left : parent.left
                right : parent.right
                bottom: parent.bottom
            }
        }

        Text {
            id: cameraHeadingLabel
            text: "Camera Heading"
            color: "white"

            anchors{
                left : parent.left
                bottom : cameraHeadingSlider.top
                margins: 5
            }
        }

        CheckBox
        {
            id: allowCamDistanceInteractionCheckBox

            anchors{
                right : parent.right
                verticalCenter : cameraHeadingLabel.verticalCenter
            }

            Text {
                text: "Allow camera distance interaction"
                color: "white"
                anchors{
                    right: parent.left
                    verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            bottom : parent.verticalCenter
            right : parent.right
            margins: 30
        }

        width: childrenRect.width
        color: Qt.rgba(0.2, 0.2, 0.2, 0.65);

        Column
        {
            spacing: 5
            padding: 5
            Text {
                id: planePitchLabel
                text: "Plane Pitch"
                color: "white"
                anchors{
                    horizontalCenter: parent.horizontalCenter
                    topMargin: 10
                }
            }

            Slider {
                id: planePitchSlider
                value: 0
                from: 90
                to: -90
                orientation: Qt.Vertical

                handle: Rectangle {
                    x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - width / 2
                    y: planePitchSlider.topPadding + planePitchSlider.visualPosition * (planePitchSlider.availableHeight - height)

                    implicitWidth: 30
                    implicitHeight: 30

                    Text {
                        id: pitchValue
                        text: "0"
                        color: "black"

                        anchors{
                            horizontalCenter : parent.horizontalCenter
                            verticalCenter : parent.verticalCenter
                        }
                    }
                }

                anchors {
                    //right : parent.right
                   // top: parent.top
                   // bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }

    }
}
