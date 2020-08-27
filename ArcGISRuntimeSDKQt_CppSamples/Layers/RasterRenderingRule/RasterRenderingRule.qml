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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

RasterRenderingRuleSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                margins: 5
            }
            height: childrenRect.height
            width: childrenRect.width
            color: "silver"
            radius: 5

            GridLayout {
                columns: 2

                Label {
                    Layout.margins: 10
                    Layout.columnSpan: 2
                    Layout.alignment: Qt.AlignHCenter
                    text: "Apply a Rendering Rule"
                    font.pixelSize: 16
                }

                ComboBox {
                    id: renderingRulesCombo
                    property int modelWidth: 0
                    Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                    Layout.margins: 10
                    model: renderingRuleNames

                    onModelChanged: {
                        for (let i = 0; i < model.length; ++i) {
                            metrics.text = model[i];
                            modelWidth = Math.max(modelWidth, metrics.width);
                        }
                    }
                    TextMetrics {
                        id: metrics
                        font: renderingRulesCombo.font
                    }
                }

                Button {
                    id: applyButton
                    Layout.margins: 10
                    text: "Apply"
                    onClicked: {
                        applyRenderingRule(renderingRulesCombo.currentIndex);
                    }
                }
            }
        }
    }
}
