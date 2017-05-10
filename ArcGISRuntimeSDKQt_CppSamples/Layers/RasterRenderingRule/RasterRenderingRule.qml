// [WriteFile Name=RasterRenderingRule, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 1.4
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

RasterRenderingRuleSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                margins: 5 * scaleFactor
            }
            height: 80 * scaleFactor
            width: 200 * scaleFactor
            color: "silver"
            radius: 5 * scaleFactor

            Column {
                spacing: 10 * scaleFactor
                anchors {
                    fill: parent
                    margins: 5 * scaleFactor
                }

                Label {
                    text: "Apply a Rendering Rule"
                    font.pixelSize: 16 * scaleFactor
                }

                Row {
                    spacing: 5 * scaleFactor

                    ComboBox {
                        id: renderingRulesCombo
                        width: 130 * scaleFactor
                        model: renderingRuleNames
                    }

                    Button {
                        id: applyButton
                        text: "Apply"
                        width: 50 * scaleFactor
                        onClicked: {
                            applyRenderingRule(renderingRulesCombo.currentIndex);
                        }
                    }
                }
            }
        }
    }
}
