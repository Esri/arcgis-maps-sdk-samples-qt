// [WriteFile Name=Hillshade_Renderer, Category=Layers]
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

import QtQuick 2.5
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2
import Esri.ArcGISRuntime 100.3

Rectangle {
    id: root
    color: "transparent"
    visible: false

    RadialGradient {
        anchors.fill: parent
        opacity: 0.7
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgrey" }
            GradientStop { position: 0.7; color: "black" }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: mouse.accepted = true
        onWheel: wheel.accepted = true
    }

    Rectangle {
        anchors.centerIn: parent
        width: 225 * scaleFactor
        height: 185 * scaleFactor
        color: "lightgrey"
        radius: 5
        border {
            color: "#4D4D4D"
            width: 1
        }

        Column {
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 15 * scaleFactor

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Hillshade Renderer Settings"
                font.weight: Font.DemiBold
            }

            Row {
                spacing: 5 * scaleFactor

                Text {
                    width: 75 * scaleFactor
                    text: "Altitude"
                }

                Slider {
                    id: altitudeSlider
                    width: 125 * scaleFactor
                    minimumValue: 0
                    maximumValue: 90
                }
            }

            Row {
                spacing: 5 * scaleFactor

                Text {
                    width: 75 * scaleFactor
                    text: "Azimuth"
                }

                Slider {
                    id: azimuthSlider
                    width: 125 * scaleFactor
                    minimumValue: 0
                    maximumValue: 360
                }
            }

            Row {
                spacing: 5 * scaleFactor

                Text {
                    width: 75 * scaleFactor
                    text: "Slope"
                }

                ComboBox {
                    id: slopeBox
                    width: 125 * scaleFactor
                    model: HillshadeSlopeTypeModel{}
                    textRole: "name"
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Apply"
                onClicked: {
                    var altitude = altitudeSlider.value;
                    var azimuth = azimuthSlider.value;
                    var slope = slopeBox.model.get(slopeBox.currentIndex).value;
                    applyHillshadeRenderer(altitude, azimuth, slope);
                    root.visible = false;
                }
            }
        }
    }
}
