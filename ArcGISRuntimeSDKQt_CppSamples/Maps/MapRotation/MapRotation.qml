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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

MapRotationSample {
    id: mapRotationSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // Slider UI presentation at bottom
    Rectangle {
        height: 46 * scaleFactor
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

        // sliderCombo: A slider and text for its value
        Row {
            id: sliderCombo
            anchors {
                centerIn: parent
            }
            spacing: 5

            Slider {
                id: slider1
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 200 * scaleFactor
                        implicitHeight: 8 * scaleFactor
                        color: "gray"
                        opacity: 0.7
                        radius: 8 * scaleFactor
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "white" : "lightgray"
                        border.color: "gray"
                        border.width: 2 * scaleFactor
                        implicitWidth: 34 * scaleFactor
                        implicitHeight: 34 * scaleFactor
                        radius: 12 * scaleFactor
                    }
                }
                // Slider controls degrees of rotation
                minimumValue: 0.0
                maximumValue: 360.0
                onPressedChanged: {
                    // Call C++ invokable function to change the rotation of the map view
                    mapRotationSample.setMapViewRotation(value);
                }
            }

            TextField {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                horizontalAlignment: TextInput.AlignHCenter
                readOnly: true
                style: TextFieldStyle {
                    textColor: "white"
                    background: Rectangle {
                        implicitWidth: 55 * scaleFactor
                        implicitHeight: 34 * scaleFactor
                        border.width: 0
                        radius: 5 * scaleFactor
                        color: "gray"
                        opacity: 0.7
                    }
                }
                font.pixelSize: 20 * scaleFactor
                text: slider1.value.toFixed(0)
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}


