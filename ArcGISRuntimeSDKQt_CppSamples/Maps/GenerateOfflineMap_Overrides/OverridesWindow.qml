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

import QtQuick 2.6
import QtQuick.Controls 2.2

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

    color: "#D6D6D6"

    Text {
        id: title
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 16 * scaleFactor
        }
        text: "Layer specific overrides"
        font{
            bold: true
            underline: true
            pixelSize: 18 * scaleFactor
        }
        color: "#474747"
    }

    ScrollView {
        id: scrollView
        anchors {
            top: title.bottom
            left: overridesPanel.left
            right: overridesPanel.right
            bottom: takeOfflineButton.top
            margins: 16 * scaleFactor
        }
        clip: true
        ScrollBar.vertical.interactive: true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        Rectangle {
            implicitHeight: childrenRect.height
            implicitWidth: overridesPanel.width
            color: "transparent"

            Text {
                id: lodTitle
                text: "Basemap Levels of Detail:"
                anchors{
                    topMargin: 16 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }
                color: "#474747"
            }

            Row {
                id: lodRange
                anchors{
                    top: lodTitle.bottom
                    topMargin: 8 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "(Least detail)"
                    font{
                        pixelSize: 12 * scaleFactor
                    }
                    color: "#474747"
                }

                RangeSlider {
                    id: lodsSlider
                    from: 0
                    to: 24
                    width: overridesPanel.width * 0.5
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

                Text {
                    text: "(Most detail)"
                    font{
                        pixelSize: 12 * scaleFactor
                    }
                    color: "#474747"
                }
            }

            Text {
                id: basemapBufferLabel
                text: "Basemap Buffer (m):"
                anchors{
                    top: lodRange.bottom
                    topMargin: 32 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }
                color: "#474747"
            }

            SpinBox {
                id: basemapBufferSB
                anchors{
                    top: basemapBufferLabel.bottom
                    topMargin: 8 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                from: 0
                to: 500
                stepSize: 50

                font.pixelSize: 12 * scaleFactor
                onValueChanged: basemapBufferChanged(value);
            }

            Button {
                id: systemVavlesCB
                text: "Remove System Valves"
                anchors{
                    top: basemapBufferSB.bottom
                    topMargin: 32 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }


                onClicked: {
                    removeSystemValvesChanged();
                    enabled = false;
                }
            }

            Button {
                id: serviceConnCB
                text: "Remove Service Connection"
                anchors{
                    top: systemVavlesCB.bottom
                    topMargin: 32 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }

                onClicked: {
                    removeServiceConnectionChanged();
                    enabled = false;
                }
            }

            Text {
                id: filterLabel
                text: "Filter Hydrants:"
                anchors{
                    top: serviceConnCB.bottom
                    topMargin: 32 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }
                color: "#474747"
            }

            Rectangle {
                anchors.fill: filterTextEdit
                color: "white"
            }

            TextInput {
                id: filterTextEdit
                anchors{
                    top: filterLabel.bottom
                    topMargin: 8 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                width: filterLabel.width

                text: "FLOW < 500"
                horizontalAlignment: Text.AlignHCenter

                onEditingFinished: hydrantWhereClauseChanged(text);
            }

            CheckBox {
                id: clipCB
                text: "Clip Water Pipes to AOI"
                anchors{
                    top: filterTextEdit.bottom
                    topMargin: 32 * scaleFactor
                    horizontalCenter: parent.horizontalCenter
                }
                font{
                    pixelSize: 14 * scaleFactor
                }

                checked: true

                onCheckedChanged: clipWaterPipesAOIChanged(checked)
            }
        }
    }

    Button {
        id: takeOfflineButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 16 * scaleFactor
        text: "Start Job"
        font{
            bold: true
            pixelSize: 18 * scaleFactor
        }

        onClicked: {
            overridesAccepted();
            parent.visible = false;
        }
    }
}

