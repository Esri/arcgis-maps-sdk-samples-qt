// [WriteFile Name=ShowInteractiveViewshedWithAnalysisOverlay, Category=Analysis]
// [Legal]
// Copyright 2026 Esri.

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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Item {
    readonly property bool compactUi: width < 760

    // Declare the C++ instance which creates the map and analysis setup.
    ShowInteractiveViewshedWithAnalysisOverlaySample {
        id: model
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
        focus: true
    }

    Component.onCompleted: {
        model.mapView = view
    }

    // Settings panel with controls for viewshed parameters.
    Rectangle {
        id: settingsPanel
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: compactUi ? 10 : 16
        anchors.rightMargin: compactUi ? 10 : 16
        radius: 12
        color: palette.base
        border.color: "black"
        border.width: 2
        opacity: 0.85
        readonly property int panelPadding: compactUi ? 10 : 14
        width: Math.min(settingsColumn.implicitWidth + panelPadding * 2, parent.width - anchors.rightMargin * 2)
        height: settingsColumn.implicitHeight + panelPadding * 2

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ColumnLayout {
            id: settingsColumn
            anchors.fill: parent
            anchors.margins: settingsPanel.panelPadding
            spacing: compactUi ? 4 : 6

            ColumnLayout {
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Observer Elevation:")
                    font.bold: true
                    Layout.preferredWidth: 180
                }

                RowLayout {
                    spacing: compactUi ? 4 : 10

                    Slider {
                        Layout.fillWidth: true
                        Layout.preferredWidth: compactUi ? 120 : 220
                        from: 2
                        to: 200
                        stepSize: 1
                        enabled: model.initialized
                        value: model.observerElevation
                        onValueChanged: model.observerElevation = value
                    }
                    Label {
                        Layout.preferredWidth: compactUi ? 52 : 64
                        horizontalAlignment: Text.AlignRight
                        text: qsTr(Math.round(model.observerElevation) + " m")
                    }
                }
            }

            ColumnLayout {
                visible: model.initialized
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Target Height:")
                    font.bold: true
                    Layout.preferredWidth: 180
                }

                RowLayout {
                    spacing: compactUi ? 4 : 10

                    Slider {
                        Layout.fillWidth: true
                        Layout.preferredWidth: compactUi ? 120 : 220
                        from: 2
                        to: 1000
                        stepSize: 1
                        value: model.targetHeight
                        onValueChanged: model.targetHeight = value
                    }
                    Label {
                        Layout.preferredWidth: compactUi ? 52 : 64
                        horizontalAlignment: Text.AlignRight
                        text: qsTr(Math.round(model.targetHeight) + " m")
                    }
                }
            }

            ColumnLayout {
                visible: model.initialized
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Maximum Radius:")
                    font.bold: true
                    Layout.preferredWidth: 180
                }

                RowLayout {
                    spacing: compactUi ? 4 : 10

                    Slider {
                        Layout.fillWidth: true
                        Layout.preferredWidth: compactUi ? 120 : 220
                        from: 250
                        to: 20000
                        stepSize: 10
                        value: model.maxRadius
                        onValueChanged: model.maxRadius = value
                    }
                    Label {
                        Layout.preferredWidth: compactUi ? 52 : 64
                        horizontalAlignment: Text.AlignRight
                        text: qsTr(Math.round(model.maxRadius) + " m")
                    }
                }
            }

            ColumnLayout {
                visible: model.initialized
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Field of View:")
                    font.bold: true
                    Layout.preferredWidth: 180
                }

                RowLayout {
                    spacing: compactUi ? 4 : 10

                    Slider {
                        Layout.fillWidth: true
                        Layout.preferredWidth: compactUi ? 120 : 220
                        from: 5
                        to: 360
                        stepSize: 1
                        value: model.fieldOfView
                        onValueChanged: model.fieldOfView = value
                    }
                    Label {
                        Layout.preferredWidth: compactUi ? 52 : 64
                        horizontalAlignment: Text.AlignRight
                        text: qsTr(Math.round(model.fieldOfView) + "\u00B0")
                    }
                }
            }

            ColumnLayout {
                visible: model.initialized
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Heading:")
                    font.bold: true
                    Layout.preferredWidth: 180
                }

                RowLayout {
                    spacing: compactUi ? 4 : 10

                    Slider {
                        Layout.fillWidth: true
                        Layout.preferredWidth: compactUi ? 120 : 220
                        from: 0
                        to: 360
                        stepSize: 1
                        value: model.heading
                        onValueChanged: model.heading = value
                    }
                    Label {
                        Layout.preferredWidth: compactUi ? 52 : 64
                        horizontalAlignment: Text.AlignRight
                        text: qsTr(Math.round(model.heading) + "\u00B0")
                    }
                }
            }

            ColumnLayout {
                visible: model.initialized
                spacing: compactUi ? 2 : 6
                Label {
                    text: qsTr("Elevation Sampling Interval (m):")
                    font.bold: true
                    Layout.preferredWidth: implicitWidth + 8
                }

                ButtonGroup {
                    id: samplingGroup
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight
                    spacing: compactUi ? 4 : 8

                    Item {
                        Layout.fillWidth: true
                    }

                    RadioButton {
                        checked: model.elevationSamplingInterval === 0
                        ButtonGroup.group: samplingGroup
                        text: qsTr("0")
                        onClicked: model.elevationSamplingInterval = 0
                    }

                    RadioButton {
                        checked: model.elevationSamplingInterval === 10
                        ButtonGroup.group: samplingGroup
                        text: qsTr("10")
                        onClicked: model.elevationSamplingInterval = 10
                    }

                    RadioButton {
                        checked: model.elevationSamplingInterval === 20
                        ButtonGroup.group: samplingGroup
                        text: qsTr("20")
                        onClicked: model.elevationSamplingInterval = 20
                    }
                }
            }
        }
    }

    Label {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16
        anchors.topMargin: 10
        text: qsTr("Raster data Copyright Scottish Government and SEPA (2014)")
        font.italic: true
        font.pointSize: 15
        color: palette.text
        font.bold: true
    }
}
