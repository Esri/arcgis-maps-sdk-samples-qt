// [WriteFile Name=Surface_Placement, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Layouts 1.12

SurfacePlacementSample {
    width: 800
    height: 600

    // Create SceneView here, and create its Scene and GraphicsOverlay in C++ code
    //! [add a sceneView component]
    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            Text {
                Layout.margins: 2
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Draped mode")
                color: "white"
            }

            Switch {
                id: surfacePlacementMode
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2

                onCheckedChanged: changeDrapedVisibility();
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            Text {
                id: zValueSliderLabel
                text: qsTr("Z-Value")
                color: "white"
                Layout.margins: 2
                Layout.alignment: Qt.AlignHCenter
            }

            Slider {
                id: zValueSlider
                from: 0
                to: 140
                value: 70
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
                orientation: Qt.Vertical

                onMoved: changeZValue(value);

                // Custom slider handle that displays the current value
                handle: Item {
                    x: parent.leftPadding + parent.availableWidth / 2 - headingHandleNub.width / 2
                    y: parent.topPadding + parent.visualPosition * (parent.availableHeight - headingHandleNub.height)

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
                        x: headingHandleNub.x - width
                        y: headingHandleNub.y - height / 2 + headingHandleNub.height / 2
                        color: zValueSlider.background.children[0].color

                        Text {
                            id: headingValue
                            font.pixelSize: 14
                            padding: 3
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            text: (zValueSlider.value).toFixed(0)
                            color: "white"
                        }
                    }
                }
            }
        }
    }
    //! [add a sceneView component]
}
