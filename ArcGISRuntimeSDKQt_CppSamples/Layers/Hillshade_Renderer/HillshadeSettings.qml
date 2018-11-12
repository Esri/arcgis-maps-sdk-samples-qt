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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

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
        width: childrenRect.width
        height: childrenRect.height
        color: "lightgrey"
        radius: 5
        border {
            color: "#4D4D4D"
            width: 1
        }

        GridLayout {
            columns: 2

            Text {
                Layout.margins: 5
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                text: "Hillshade Renderer Settings"
                font.weight: Font.DemiBold
            }

            Text {
                Layout.margins: 5
                text: "Altitude"
            }

            Slider {

                id: altitudeSlider
                Layout.margins: 5
                from: 0
                to: 90
            }

            Text {
                Layout.margins: 5
                text: "Azimuth"
            }

            Slider {
                id: azimuthSlider
                Layout.margins: 5
                from: 0
                to: 360
            }

            Text {
                Layout.margins: 5
                text: "Slope"
            }

            ComboBox {
                id: slopeBox
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.margins: 5
                Layout.fillWidth: true
                model: HillshadeSlopeTypeModel{}
                textRole: "name"

                Component.onCompleted : {
                    for (var i = 0; i < model.count; ++i) {
                        metrics.text = model.get(i).name;
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }
                TextMetrics {
                    id: metrics
                    font: slopeBox.font
                }
            }

            Button {
                Layout.margins: 5
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
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
