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

    Item {
        anchors.fill: parent

        Rectangle {
            id: menuBackground
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                margins: 16
            }
            width: Math.min(Math.max(parent.width * 0.6, 300), Math.min(500, parent.width - 32))
            height: layerColumn.implicitHeight + 24
            color: palette.mid
            radius: 8

            ColumnLayout {
                id: layerColumn
                anchors {
                    fill: parent
                    margins: 12
                }
                spacing: 12

                Label {
                    id: title
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 8
                    text: qsTr("Layer specific overrides")
                    font {
                        bold: true
                        underline: true
                        pixelSize: 18
                    }
                }

                ScrollView {
                    id: scrollView
                    Layout.fillWidth: true
                    implicitHeight: Math.min(contentColumn.implicitHeight, overridesPanel.height - title.height - takeOfflineButton.height - 100)
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ColumnLayout {
                        id: contentColumn
                        width: scrollView.width
                        spacing: 12
                        Layout.leftMargin: 4
                        Layout.rightMargin: 4

                        GroupBox {
                            Layout.fillWidth: true
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            title: qsTr("Basemap Levels of Detail")
                            font.pixelSize: 14

                            ColumnLayout {
                                width: parent.width
                                spacing: 4

                                Label {
                                    Layout.alignment: Qt.AlignLeft
                                    text: qsTr("(Least detail)")
                                    font.pixelSize: 11
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
                                    text: qsTr("(Most detail)")
                                    font.pixelSize: 11
                                }
                            }
                        }

                        GroupBox {
                            Layout.fillWidth: true
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            title: qsTr("Basemap Buffer (m)")
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
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            title: qsTr("Layer Filters")
                            font.pixelSize: 14

                            ColumnLayout {
                                width: parent.width
                                spacing: 8

                                Button {
                                    id: systemVavlesCB
                                    Layout.fillWidth: true
                                    text: qsTr("Remove System Valves")
                                    font.pixelSize: 13

                                    onClicked: {
                                        removeSystemValvesChanged();
                                        enabled = false;
                                    }
                                }

                                Button {
                                    id: serviceConnCB
                                    Layout.fillWidth: true
                                    text: qsTr("Remove Service Connection")
                                    font.pixelSize: 13

                                    onClicked: {
                                        removeServiceConnectionChanged();
                                        enabled = false;
                                    }
                                }
                            }
                        }

                        GroupBox {
                            Layout.fillWidth: true
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            title: qsTr("Filter Hydrants")
                            font.pixelSize: 14

                            ComboBox {
                                id: filterComboBox
                                anchors.horizontalCenter: parent.horizontalCenter
                                property int bestWidth: 100
                                model: [ "No filter", "FLOW < 500", "FLOW < 300", "FLOW < 100" ]
                                font.pixelSize: 12
                                width: bestWidth + leftPadding + rightPadding + (indicator ? indicator.width : 0) + 40

                                onCurrentTextChanged: {
                                    // 1=1 equivelent to select all in a WHERE clause.
                                    hydrantWhereClauseChanged(currentText === "No filter" ? "1=1"
                                                                                          : currentText)
                                }

                                Component.onCompleted: {
                                    let w = 0;
                                    for (let i = 0; i < model.length; ++i) {
                                        metrics.text = model[i];
                                        w = Math.max(w, metrics.width);
                                    }
                                    bestWidth = w;
                                }

                                TextMetrics {
                                    id: metrics
                                    font: filterComboBox.font
                                }
                            }
                        }

                        GroupBox {
                            Layout.fillWidth: true
                            Layout.leftMargin: 4
                            Layout.rightMargin: 4
                            title: qsTr("Clipping Options")
                            font.pixelSize: 14

                            CheckBox {
                                id: clipCB
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: qsTr("Clip Water Pipes to AOI")
                                font.pixelSize: 13
                                checked: true

                                onCheckedChanged: clipWaterPipesAOIChanged(checked)
                            }
                        }
                    }
                }

                Button {
                    id: takeOfflineButton
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 8
                    text: qsTr("Start Job")
                    font.pixelSize: 14

                    onClicked: {
                        overridesAccepted();
                        layerColumn.visible = false;
                    }
                }
            }
        }
    }
}
