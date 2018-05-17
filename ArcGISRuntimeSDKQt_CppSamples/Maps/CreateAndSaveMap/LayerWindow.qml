// Copyright 2018 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

Rectangle {
    RadialGradient {
        anchors.fill: parent
        opacity: 0.5
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.7; color: "black" }
        }
    }

    signal createMapSelected(var basemap, var layerList)

    Rectangle {
        anchors {
            fill: layerColumn
            margins: -15 * scaleFactor
        }

        color: "#edeeef"
        radius: 5 * scaleFactor
        border {
            color: "#77787a"
            width: 1 * scaleFactor
        }
    }

    Column {
        id: layerColumn
        anchors.centerIn: parent
        width: 200 * scaleFactor

        spacing: 10 * scaleFactor

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Select Layers"
            font {
                pixelSize: 18 * scaleFactor
                family: "helvetica"
            }
        }

        Text {
            text: "Select Basemap:"
            font {
                pixelSize: 14 * scaleFactor
                family: "helvetica"
            }
        }

        ComboBox {
            id: basemapComboBox
            width: parent.width
            model: ["Streets", "Imagery", "Topographic", "Oceans"]
        }

        Text {
            text: "Select Operational Layers:"
            font {
                pixelSize: 14 * scaleFactor
                family: "helvetica"
            }
        }

        Repeater {
            id: operationalLayerRepeater
            width: parent.width
            model: ["WorldElevations", "Census"]

            CheckBox {
                text: modelData
                checked: true
            }
        }

        Button {
            width: parent.width
            text: "Create Map"
            onClicked: {
                var layerList = [];
                for (var i = 0; i < operationalLayerRepeater.count; i++) {
                    var currentCheckbox = operationalLayerRepeater.itemAt(i);
                    if (currentCheckbox.checked) {
                        layerList.push(currentCheckbox.text)
                    }
                }
                createMapSelected(basemapComboBox.currentText, layerList);
            }
        }
    }
}
