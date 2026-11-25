// [WriteFile Name=RasterRenderingRule, Category=Layers]
// [Legal]
// Copyright 2017 Esri.
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

RasterRenderingRuleSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                margins: 5
            }
            width: Math.min(controlsLayout.implicitWidth, parent.width - 10)
            height: controlsLayout.implicitHeight
            color: palette.base
            radius: 5

            GridLayout {
                id: controlsLayout
                width: parent.width
                columns: 2

                Label {
                    Layout.margins: 10
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("Apply a Rendering Rule")
                    font.pixelSize: 16
                }

                ComboBox {
                    id: renderingRulesCombo
                    property int modelWidth: 0
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: modelWidth + leftPadding + rightPadding +
                                           (indicator ? indicator.width : 10)
                    Layout.maximumWidth: rootRectangle.width - applyButton.implicitWidth - 50
                    Layout.margins: 10
                    model: renderingRuleNames

                    onModelChanged: {
                        modelWidth = 0;
                        for (let index = 0; index < model.length; ++index) {
                            optionMetrics.text = model[index];
                            modelWidth = Math.max(modelWidth, optionMetrics.width);
                        }
                    }

                    TextMetrics {
                        id: optionMetrics
                        font: renderingRulesCombo.font
                    }
                }

                Button {
                    id: applyButton
                    Layout.margins: 10
                    text: qsTr("Apply")
                    onClicked: {
                        applyRenderingRule(renderingRulesCombo.currentIndex);
                    }
                }
            }
        }
    }
}
