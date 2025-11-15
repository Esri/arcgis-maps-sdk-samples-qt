// [WriteFile Name=OrbitCameraAroundObject, Category=Scenes]
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

import Esri.Samples
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
    OrbitCameraAroundObjectSample {
        id: model
        sceneView: view
        planePitch: planePitchSlider.value
    }

    //Camera heading slider, sits at the bottom of the screen
    Rectangle {
        id: cameraHeadingContainer
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 20
        }
        width: parent.width * 0.5
        height: 60
        color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
        radius: 5
        border {
            width: 1
            color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"
        }

        Slider {
            id: cameraHeadingSlider
            implicitHeight: 40
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 10
            }

            value: model.cameraHeading
            from: model.cameraHeadingBounds.x
            to: model.cameraHeadingBounds.y

            onMoved: {
                model.cameraHeading = cameraHeadingSlider.value
            }

            background: Rectangle {
                x: cameraHeadingSlider.leftPadding
                y: cameraHeadingSlider.topPadding + cameraHeadingSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 2
                width: cameraHeadingSlider.availableWidth
                height: implicitHeight
                color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"

                Rectangle {
                    width: cameraHeadingSlider.visualPosition * parent.width
                    height: parent.height
                    color: palette.highlight
                }
            }

            handle: Item {
                x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - headingHandleNub.width)
                y: parent.topPadding + parent.availableHeight / 2 - headingHandleNub.height / 2

                Rectangle {
                    id: headingHandleNub
                    color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
                    radius: width * 0.5
                    width: 20
                    height: width
                    border.width: 2
                    border.color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#757575" : "#949494"
                }
                Rectangle {
                    id: headingHandleRect
                    height: childrenRect.height
                    width: childrenRect.width
                    radius: 3
                    x: headingHandleNub.x - width / 2 + headingHandleNub.width / 2
                    y: headingHandleNub.y - height - 5
                    color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
                    border.width: 1
                    border.color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"

                    Label {
                        id: headingValue
                        font.pixelSize: 16
                        font.bold: true
                        padding: 5
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        text: qsTr(Math.round(cameraHeadingSlider.value) + "\u00B0")
                        color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#FFFFFF" : "#151515"
                    }
                }
            }
        }

        Label {
            id: cameraHeadingLabel
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 5
            }
            text: qsTr("Camera Heading")
            font.pixelSize: 12
            color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#BFBFBF" : "#4A4A4A"
        }
    }

    //Plane pitch slider, placed in the top-right of the screen
    Rectangle {
        id: planePitchContainer
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 20
        }
        width: 60
        height: parent.height * 0.5
        color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
        radius: 5
        border {
            width: 1
            color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"
        }

        Label {
            id: planePitchLabel
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            text: qsTr("Plane\nPitch")
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#BFBFBF" : "#4A4A4A"
        }

        Slider {
            id: planePitchSlider
            implicitWidth: 40
            anchors {
                top: planePitchLabel.bottom
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                topMargin: 5
                bottomMargin: 10
            }

            value: 0
            from: 90
            to: -90
            orientation: Qt.Vertical

            background: Rectangle {
                x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - width / 2
                y: planePitchSlider.topPadding
                implicitWidth: 2
                implicitHeight: 160
                width: implicitWidth
                height: planePitchSlider.availableHeight
                color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"

                Rectangle {
                    y: planePitchSlider.visualPosition * parent.height
                    width: parent.width
                    height: planePitchSlider.position * parent.height
                    color: palette.highlight
                }
            }

            //Custom slider handle that displays the current value
            handle: Item {
                x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - pitchHandleNub.width / 2
                y: planePitchSlider.topPadding + planePitchSlider.visualPosition * (planePitchSlider.availableHeight - pitchHandleNub.height)

                Rectangle {
                    id: pitchHandleNub
                    color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
                    radius: width * 0.5
                    width: 20
                    height: width
                    border.width: 2
                    border.color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#757575" : "#949494"
                }
                Rectangle {
                    id: pitchHandleRect
                    height: childrenRect.height
                    width: childrenRect.width
                    radius: 3
                    x: pitchHandleNub.x - width - 5
                    y: pitchHandleNub.y - height/2 + pitchHandleNub.height/2
                    color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
                    border.width: 1
                    border.color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"

                    Label {
                        id: pitchValue
                        font.pixelSize: 16
                        font.bold: true
                        padding: 5
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        text: qsTr(Math.round(planePitchSlider.value)  + "\u00B0")
                        color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#FFFFFF" : "#151515"
                    }
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
        height: viewOptionsColumn.implicitHeight + 16
        width: viewOptionsColumn.implicitWidth + 16
        color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#2B2B2B" : "#FFFFFF"
        border{
            width: 1
            color: Qt.application.styleHints.colorScheme === Qt.ColorScheme.Dark ? "#404040" : "#DFDFDF"
        }

        Column {
            id: viewOptionsColumn
            spacing: 10
            anchors.centerIn: parent

            Button {
                text: qsTr("Cockpit View")
                implicitWidth: 180
                onClicked: {
                    model.cockpitView();
                    allowCamDistanceInteractionCheckBox.enabled = false;
                }
            }

            Button {
                text: qsTr("Center View")
                implicitWidth: 180
                onClicked: {
                    model.centerView();
                    allowCamDistanceInteractionCheckBox.enabled = true;
                }
            }

            CheckBox {
                id: allowCamDistanceInteractionCheckBox
                implicitWidth: 180
                checked: model.allowCamDistanceInteraction
                onCheckedChanged: model.allowCamDistanceInteraction = checked
                text: qsTr("Allow camera\ndistance interaction")
            }
        }
    }
}
