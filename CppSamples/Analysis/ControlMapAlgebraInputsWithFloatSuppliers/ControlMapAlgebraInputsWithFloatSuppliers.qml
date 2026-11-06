// [WriteFile Name=ControlMapAlgebraInputsWithFloatSuppliers, Category=Analysis]
// [Legal]
// Copyright 2026 Esri.
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
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Esri.Samples

Item {
    readonly property bool compactUi: width < 760

    ControlMapAlgebraInputsWithFloatSuppliersSample {
        id: model
    }

    MapView {
        id: view
        anchors.fill: parent
        objectName: "mapView"
        focus: true
    }

    Component.onCompleted: model.mapView = view

    Rectangle {
        id: settingsPanel
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: compactUi ? 8 : 18
            rightMargin: compactUi ? 8 : 18
            bottomMargin: compactUi ? 8 : 18
        }
        height: settingsColumn.implicitHeight + panelVerticalPadding * 2
        radius: 8
        color: palette.base
        border.color: palette.mid
        opacity: 0.94
        z: 2
        clip: true
        readonly property int panelHorizontalPadding: compactUi ? 10 : 16
        readonly property int panelVerticalPadding: compactUi ? 8 : 12

        // accept mouse events so they do not propagate down to the map
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ColumnLayout {
            id: settingsColumn
            anchors {
                fill: parent
                leftMargin: settingsPanel.panelHorizontalPadding
                rightMargin: settingsPanel.panelHorizontalPadding
                topMargin: settingsPanel.panelVerticalPadding
                bottomMargin: settingsPanel.panelVerticalPadding
            }
            spacing: compactUi ? 4 : 6
            clip: true

            RowLayout {
                Layout.fillWidth: true

                Label {
                    Layout.minimumWidth: 0
                    text: qsTr("Slope range")
                    font.bold: true
                    elide: Text.ElideRight
                }

                Item {
                    Layout.fillWidth: true
                }

                Label {
                    Layout.minimumWidth: 0
                    text: qsTr("%1 to %2 degrees").arg(model.minimumSlope).arg(model.maximumSlope)
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideLeft
                }
            }

            RangeSlider {
                id: slopeRange
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                from: 0
                to: 85
                stepSize: 1
                first.value: model.minimumSlope
                second.value: model.maximumSlope

                first.onMoved: model.setSlopeRange(Math.round(first.value), Math.round(second.value))
                second.onMoved: model.setSlopeRange(Math.round(first.value), Math.round(second.value))

                background: Rectangle {
                    x: slopeRange.leftPadding
                    y: slopeRange.topPadding + slopeRange.availableHeight / 2 - height / 2
                    width: slopeRange.availableWidth
                    height: 8
                    radius: 4
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop {
                            position: 0.0
                            color: "#440154"
                        }
                        GradientStop {
                            position: 0.2
                            color: "#414487"
                        }
                        GradientStop {
                            position: 0.4
                            color: "#2A788E"
                        }
                        GradientStop {
                            position: 0.6
                            color: "#22A884"
                        }
                        GradientStop {
                            position: 0.8
                            color: "#7AD151"
                        }
                        GradientStop {
                            position: 1.0
                            color: "#FDE725"
                        }
                    }
                }
            }

            Item {
                id: rangeFooter
                Layout.fillWidth: true
                implicitHeight: Math.max(flattestLabel.implicitHeight, updatingStatus.implicitHeight, steepestLabel.implicitHeight)

                Label {
                    id: flattestLabel
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width * 0.25
                    text: qsTr("Flattest")
                    elide: Text.ElideRight
                }

                Item {
                    id: updatingStatus
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: parent.width * 0.25
                        rightMargin: parent.width * 0.25
                        verticalCenter: parent.verticalCenter
                    }
                    height: implicitHeight
                    implicitHeight: Math.max(updatingIndicator.implicitHeight, updatingLabel.implicitHeight)
                    visible: model.updating

                    Label {
                        id: updatingLabel
                        anchors.centerIn: parent
                        width: Math.max(0, parent.width - (updatingIndicator.visible ? 2 * (updatingIndicator.width + 4) : 0))
                        text: qsTr("Analysis updating")
                        elide: Text.ElideRight
                        horizontalAlignment: Text.AlignHCenter
                    }

                    BusyIndicator {
                        id: updatingIndicator
                        anchors {
                            right: updatingLabel.left
                            rightMargin: 4
                            verticalCenter: parent.verticalCenter
                        }
                        visible: parent.width >= updatingLabel.implicitWidth + 2 * (width + 4)
                        running: model.updating
                        implicitWidth: 22
                        implicitHeight: 22
                    }
                }

                Label {
                    id: steepestLabel
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width * 0.25
                    text: qsTr("Steepest")
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignRight
                }
            }

            Label {
                visible: model.errorMessage.length > 0
                text: model.errorMessage
                color: palette.text
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.minimumWidth: 0
            }
        }
    }
}
