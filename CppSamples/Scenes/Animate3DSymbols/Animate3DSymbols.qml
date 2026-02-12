// [WriteFile Name=Animate3DSymbols, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.
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

Animate3DSymbolsSample {
    id: rootRectangle

    property bool following: followButton.checked

    missionFrame: progressSlider.value
    zoom: cameraDistance.value
    angle: cameraAngle.value

    onNextFrameRequested: {
        progressSlider.value = progressSlider.value + 1;
        if (progressSlider.value >= missionSize)
            progressSlider.value = 0;
    }

    Component.onCompleted: {
        missionList.currentIndex = 0;
    }

    SceneView {
        id: sceneView
        objectName: "sceneView"
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Left menu - Mission selector and controls
        Pane {
            id: leftPane
            padding: 10
            anchors {
                left: parent.left
                top: parent.top
                margins: 10
            }
            width: children.implicitWidth

            ColumnLayout {
                spacing: 10
                width: children.implicitWidth

                ComboBox {
                    id: missionList
                    enabled: !playButton.checked
                    model: missionsModel
                    textRole: "display"
                    property real modelWidth: 0
                    Layout.fillWidth: true

                    onModelChanged: {
                        for (let i = 0; i < missionsModel.rowCount(); ++i) {
                            const index = missionsModel.index(i, 0);
                            textMetrics.text = missionsModel.data(index);
                            modelWidth = Math.max(modelWidth, textMetrics.width);
                        }
                    }

                    onCurrentTextChanged: {
                        changeMission(currentText);
                        progressSlider.value = 0;
                    }

                    TextMetrics {
                        id: textMetrics
                        font: missionList.font
                    }

                    Component.onCompleted: missionList.currentTextChanged()
                }

                Button {
                    id: playButton
                    checked: false
                    checkable: true
                    enabled: missionReady
                    text: checked ? qsTr("pause") : qsTr("play")
                    Layout.fillWidth: true
                }

                Button {
                    id: followButton
                    enabled: missionReady
                    text: checked ? qsTr("fixed") : qsTr("follow")
                    checked: true
                    checkable: true
                    onCheckedChanged: setFollowing(checked);
                    Layout.fillWidth: true
                }
            }
        }

        // Right menu - Slider controls
        Pane {
            id: sliderPane
            padding: 10
            anchors {
                top: parent.top
                right: parent.right
                margins: 10
            }
            width: children.implicitWidth

            ColumnLayout {
                spacing: 15
                anchors.fill: parent

                // Zoom Slider
                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true
                    Layout.maximumWidth: parent.width

                    Label {
                        text: qsTr("Zoom")
                        font.bold: true
                        Layout.maximumWidth: parent.width
                        elide: Text.ElideRight
                    }
                    Slider {
                        id: cameraDistance
                        from: 10.0
                        to: 5000.0
                        value: 500.0
                        Layout.fillWidth: true
                        Layout.maximumWidth: parent.width
                    }
                }

                // Camera Angle Slider
                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Angle")
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Slider {
                        id: cameraAngle
                        from: 0
                        to: 180.0
                        value: 45.0
                        Layout.fillWidth: true
                    }
                }

                // Progress Slider
                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Progress: " + (progressSlider.value / missionSize * 100).toLocaleString(Qt.locale(), 'f', 0) + "%")
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Slider {
                        id: progressSlider
                        from: 0
                        to: missionSize
                        enabled: missionReady
                        Layout.fillWidth: true
                    }
                }

                // Animation Speed Slider
                ColumnLayout {
                    spacing: 4
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Speed")
                        font.bold: true
                        elide: Text.ElideRight
                    }
                    Slider {
                        id: animationSpeed
                        from: 1
                        to: 100
                        value: 50
                        Layout.fillWidth: true
                    }
                }

                // Spacer
                Item {
                    Layout.fillHeight: true
                }
            }
        }

        // Map view at bottom left
        Rectangle {
            id: mapFrame
            anchors {
                left: parent.left
                bottom: sceneView.attributionTop
                margins: 10
            }
            height: parent.height * 0.25
            width: parent.width * 0.3
            color: "black"
            clip: true

            MapView {
                id: mapView
                anchors {
                    fill: parent
                    margins: 2
                }
                objectName: "mapView"

                MouseArea {
                    anchors.fill: parent
                    onPressed: mouse => mouse.accepted
                    onWheel: wheel.accepted
                }
            }

            RowLayout {
                anchors {
                    left: parent.left
                    top: parent.top
                }
                spacing: 2

                ToolButton {
                    Layout.margins: 5
                    padding: 0
                    icon {
                        source: "qrc:/Samples/Scenes/Animate3DSymbols/minus-24.svg"
                        width: 24
                        height: 24
                        color: palette.buttonText
                    }
                    onClicked: zoomMapOut()

                    background: Rectangle {
                        implicitWidth: 24
                        implicitHeight: 24
                        color: parent.hovered ? palette.midlight : palette.button
                        opacity: 0.9
                        radius: 5
                    }
                }

                ToolButton {
                    Layout.margins: 5
                    padding: 0
                    icon {
                        source: "qrc:/Samples/Scenes/Animate3DSymbols/plus-24.svg"
                        width: 24
                        height: 24
                        color: palette.buttonText
                    }
                    onClicked: zoomMapIn()

                    background: Rectangle {
                        implicitWidth: 24
                        implicitHeight: 24
                        color: parent.hovered ? palette.midlight : palette.button
                        opacity: 0.9
                        radius: 5
                    }
                }
            }
        }
    }

    Timer {
        id: timer
        interval: 16.0 + 84 * (animationSpeed.to - animationSpeed.value) / 100.0;
        running: playButton.checked;
        repeat: true
        onTriggered: animate();
    }
}
