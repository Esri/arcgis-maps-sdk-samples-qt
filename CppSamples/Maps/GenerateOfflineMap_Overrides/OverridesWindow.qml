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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    id: overridesPanel
    visible: overridesReady
    signal basemapLODSelected(real min, real max)
    signal basemapBufferChanged(real buffer)
    signal removeSystemValvesChanged()
    signal removeServiceConnectionChanged()
    signal hydrantWhereClauseChanged(string whereClause)
    signal clipWaterPipesAOIChanged(bool clip)
    signal overridesAccepted()

    color: palette.base

    ColumnLayout {
        id: layerColumn
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 16
        }
        width: parent.width * 0.6
        spacing: 8

        Label {
            id: title
            Layout.alignment: Qt.AlignHCenter
            text: "Layer specific overrides"
            font {
                bold: true
                underline: true
                pixelSize: 18
            }
        }

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.vertical.interactive: true
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ColumnLayout {
                width: scrollView.availableWidth
                spacing: 16

                GroupBox {
                    Layout.fillWidth: true
                    title: "Basemap Levels of Detail"
                    font.pixelSize: 14

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 8

                        Label {
                            Layout.alignment: Qt.AlignLeft
                            text: "(Least detail)"
                            font.pixelSize: 12
                        }

                        RangeSlider {
                            id: lodsSlider
                            Layout.fillWidth: true
                            from: 0
                            to: 24
                            first.value: 0
                            second.value: 23
                            first.onPressedChanged: {
                                if (first.pressed)
                                    return;
                                basemapLODSelected(first.value, second.value);
                            }

                            second.onPressedChanged: {
                                if (second.pressed)
                                    return;
                                basemapLODSelected(first.value, second.value);
                            }
                        }

                        Label {
                            Layout.alignment: Qt.AlignRight
                            text: "(Most detail)"
                            font.pixelSize: 12
                        }
                    }
                }

                GroupBox {
                    Layout.fillWidth: true
                    title: "Basemap Buffer (m)"
                    font.pixelSize: 14

                    SpinBox {
                        id: basemapBufferSB
                        anchors.horizontalCenter: parent.horizontalCenter
                        from: 0
                        to: 500
                        stepSize: 50
                        font.pixelSize: 12
                        onValueChanged: basemapBufferChanged(value);
                    }
                }

                GroupBox {
                    Layout.fillWidth: true
                    title: "Layer Filters"
                    font.pixelSize: 14

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 12

                        Button {
                            id: systemVavlesCB
                            Layout.fillWidth: true
                            text: "Remove System Valves"
                            font.pixelSize: 14

                            onClicked: {
                                removeSystemValvesChanged();
                                enabled = false;
                            }
                        }

                        Button {
                            id: serviceConnCB
                            Layout.fillWidth: true
                            text: "Remove Service Connection"
                            font.pixelSize: 14

                            onClicked: {
                                removeServiceConnectionChanged();
                                enabled = false;
                            }
                        }
                    }
                }

                GroupBox {
                    Layout.fillWidth: true
                    title: "Filter Hydrants"
                    font.pixelSize: 14

                    ComboBox {
                        id: filterComboBox
                        anchors.horizontalCenter: parent.horizontalCenter
                        property int modelWidth: 0
                        width: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                        model: [ "No filter", "FLOW < 500", "FLOW < 300", "FLOW < 100" ]

                        onCurrentTextChanged: {
                            // 1=1 equivelent to select all in a WHERE clause.
                            hydrantWhereClauseChanged(currentText === "No filter" ? "1=1"
                                                                                  : currentText)
                        }

                        Component.onCompleted : {
                            for (let i = 0; i < model.length; ++i) {
                                metrics.text = model[i];
                                modelWidth = Math.max(modelWidth, metrics.width);
                            }
                        }
                        TextMetrics {
                            id: metrics
                            font: filterComboBox.font
                        }
                    }
                }

                GroupBox {
                    Layout.fillWidth: true
                    title: "Clipping Options"
                    font.pixelSize: 14

                    CheckBox {
                        id: clipCB
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Clip Water Pipes to AOI"
                        font.pixelSize: 14
                        checked: true

                        onCheckedChanged: clipWaterPipesAOIChanged(checked)
                    }
                }
            }
        }

        Button {
            id: takeOfflineButton
            Layout.alignment: Qt.AlignHCenter
            text: "Start Job"

            onClicked: {
                overridesAccepted();
                layerColumn.visible = false;
            }
        }
    }
}
