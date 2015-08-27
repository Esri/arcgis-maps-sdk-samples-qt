// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Map UI presentation at top
    MapView {
        id: mv
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: slideContainer.top
        }

        Map {
            BasemapStreets {}
            initialViewpoint: vc
        }

        ViewpointCenter {
            id: vc
            center: Point {
                x: -11070722
                y: 4658720
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
            scale: 55151630
        }
        onMapRotationChanged: {
            slider1.value = rotation
        }
    }

    // Slider UI presentation at bottom
    Rectangle {
        id: slideContainer
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: 35 * scaleFactor
        width: parent.width

        // sliderCombo: A slider and text for its value
        Row {
            id: sliderCombo
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 20 * scaleFactor

            Slider {
                id: slider1
                anchors {
                    topMargin: 50 * scaleFactor
                }
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 400
                        implicitHeight: 8
                        color: "gray"
                        radius: 8
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "white" : "lightgray"
                        border.color: "gray"
                        border.width: 2
                        implicitWidth: 34
                        implicitHeight: 34
                        radius: 12
                    }
                }
                // Slider controls degrees of rotation
                minimumValue: 0.0
                maximumValue: 360.0
                onPressedChanged: {
                    mv.setViewpointRotation(value);
                }
            }
            TextField {
                style: TextFieldStyle {
                    textColor: "black"
                    background: Rectangle {
                        radius: 2
                        implicitWidth: 50
                        implicitHeight: 35
                        border.color: slideContainer.color
                        border.width: 1
                    }
                }
                font.pointSize: 14
                text: slider1.value.toFixed(0)
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
