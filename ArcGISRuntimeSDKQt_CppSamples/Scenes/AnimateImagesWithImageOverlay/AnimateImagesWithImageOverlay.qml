// [WriteFile Name=AnimateImagesWithImageOverlay, Category=Scenes]
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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import Esri.Samples 1.0

Item {
    property var timerIntervals: [17,34,68]
    property var imageFrameRefreshRate: ["Fast","Medium","Slow"]

    SceneView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: controlsRect
            anchors {
                bottom: view.attributionTop
                horizontalCenter: parent.horizontalCenter
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"
            opacity: 0.8
            radius: 5
            ColumnLayout {
                id: controlsLayout
                Layout.alignment: Qt.AlignBottom
                spacing: 0

                Slider {
                    id: opacitySlider
                    from: 0
                    to: 1
                    value: 1
                    Layout.preferredHeight: bottomRowControls.height
                    Layout.alignment: Qt.AlignHCenter

                    onMoved: model.setOpacity(value);

                    // Custom slider handle that displays the current value
                    handle: Item {
                        x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - headingHandleNub.width)
                        y: parent.topPadding + parent.availableHeight / 2 - headingHandleNub.height / 2

                        Rectangle {
                            id: headingHandleNub
                            color: headingHandleRect.color
                            radius: width * 0.5
                            width: 20
                            height: width
                        }
                        Rectangle {
                            id: headingHandleRect
                            height: childrenRect.height
                            width: childrenRect.width
                            radius: 3
                            x: headingHandleNub.x - width / 2 + headingHandleNub.width / 2
                            y: headingHandleNub.y - height
                            color: opacitySlider.background.children[0].color

                            Text {
                                id: headingValue
                                font.pixelSize: 14
                                padding: 3
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                                text: "opacity: " + (opacitySlider.value).toFixed(2)
                                color: "white"
                            }
                        }
                    }
                }

                RowLayout {
                    id: bottomRowControls
                    spacing: 2
                    Button {
                        id: startStopBtn
                        text: model.isStopped ? qsTr("Start") : qsTr("Stop")
                        Layout.minimumWidth: timerIntervalComboBox.width
                        onClicked: {
                            if (model.isStopped) {
                                model.startTimer();
                            } else {
                                model.stopTimer();
                            }
                        }
                    }

                    ComboBox {
                        id: timerIntervalComboBox
                        currentIndex: 2
                        model: imageFrameRefreshRate
                        Layout.minimumWidth: startStopBtn.width

                        onActivated: {
                            model.setTimerInterval(timerIntervals[currentIndex]);
                        }
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    AnimateImagesWithImageOverlaySample {
        id: model
        sceneView: view
    }
}
