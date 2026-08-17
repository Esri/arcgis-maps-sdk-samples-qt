// [WriteFile Name=ApplyPointCloudRendererAndFilter, Category=Scenes]
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
import QtQuick.Controls
import Esri.Samples

Item {
    id: root

    component SectionPanel: Rectangle {
        id: sectionPanel

        required property string title
        property bool expanded: false
        default property alias content: sectionBody.data

        height: sectionContent.implicitHeight + 32
        color: palette.base
        border.color: palette.dark
        radius: 6

        Column {
            id: sectionContent
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 16
            spacing: 12

            Rectangle {
                id: sectionHeader
                width: parent.width
                height: 40
                color: "transparent"
                radius: 4

                MouseArea {
                    anchors.fill: parent
                    onClicked: sectionPanel.expanded = !sectionPanel.expanded
                }

                Label {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    text: sectionPanel.title
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }

                Label {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: sectionPanel.expanded ? "-" : "+"
                    font.pixelSize: 20
                }
            }

            Column {
                id: sectionBody
                width: parent.width
                spacing: 12
                visible: sectionPanel.expanded
            }
        }
    }

    LocalSceneView {
        id: view
        anchors.fill: parent
    }

    ApplyPointCloudRendererAndFilterSample {
        id: sampleModel
        sceneView: view
    }

    ScrollView {
        id: controlsPanel
        width: Math.min(340, root.width - 24)
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12
        anchors.bottom: parent.bottom
        clip: true
        background: Item {}

        Column {
            width: controlsPanel.availableWidth
            spacing: 12

            SectionPanel {
                width: parent.width
                title: "Rendering"
                expanded: true

                GroupBox {
                    width: parent.width
                    title: "Renderer"

                    Column {
                        width: parent.width
                        spacing: 4

                        ButtonGroup {
                            id: rendererButtonGroup
                        }

                        RadioButton {
                            text: "RGB"
                            checked: true
                            ButtonGroup.group: rendererButtonGroup
                            onClicked: {
                                sampleModel.applyRGBRenderer()
                                sampleModel.setPointSize(pointSizeSlider.value)
                            }
                        }

                        RadioButton {
                            text: "Stretch"
                            ButtonGroup.group: rendererButtonGroup
                            onClicked: {
                                sampleModel.applyStretchRenderer()
                                sampleModel.setPointSize(pointSizeSlider.value)
                            }
                        }

                        RadioButton {
                            text: "Class breaks"
                            ButtonGroup.group: rendererButtonGroup
                            onClicked: {
                                sampleModel.applyClassBreaksRenderer()
                                sampleModel.setPointSize(pointSizeSlider.value)
                            }
                        }

                        RadioButton {
                            text: "Unique value"
                            ButtonGroup.group: rendererButtonGroup
                            onClicked: {
                                sampleModel.applyUniqueValueRenderer()
                                sampleModel.setPointSize(pointSizeSlider.value)
                            }
                        }
                    }
                }

                GroupBox {
                    width: parent.width
                    title: "Point size"

                    Column {
                        width: parent.width
                        spacing: 4

                        Slider {
                            id: pointSizeSlider
                            width: parent.width
                            from: 0
                            to: 5
                            value: 1.0
                            onMoved: sampleModel.setPointSize(value)
                        }
                    }
                }

            }

            SectionPanel {
                width: parent.width
                title: "Filtering"
                expanded: false

                GroupBox {
                    width: parent.width
                    title: "Value filter"

                    Column {
                        width: parent.width
                        spacing: 4

                        Label {
                            text: "Filter mode"
                            font.weight: Font.DemiBold
                        }

                        Row {
                            width: parent.width
                            spacing: 8

                            RadioButton {
                                id: includeSelectedMode
                                text: "Include selected"
                                onClicked: {
                                    sampleModel.toggleValueFilterMode(false)
                                    sampleModel.applyValueFilter()
                                }
                            }

                            RadioButton {
                                id: excludeSelectedMode
                                text: "Exclude selected"
                                onClicked: {
                                    sampleModel.toggleValueFilterMode(true)
                                    sampleModel.applyValueFilter()
                                }
                            }
                        }

                        Repeater {
                            id: classCodeRepeater
                            model: [
                                { code: 2, label: "Ground" },
                                { code: 5, label: "High vegetation" },
                                { code: 6, label: "Building" }
                            ]

                            delegate: CheckBox {
                                required property var modelData
                                text: modelData.label
                                onToggled: {
                                    if (checked && !includeSelectedMode.checked
                                            && !excludeSelectedMode.checked) {
                                        includeSelectedMode.checked = true
                                        sampleModel.toggleValueFilterMode(false)
                                    }
                                    sampleModel.toggleClassCode(modelData.code)
                                    sampleModel.applyValueFilter()
                                }
                            }
                        }

                        Button {
                            width: parent.width
                            text: "Clear value filter"
                            onClicked: {
                                for (let index = 0; index < classCodeRepeater.count; ++index) {
                                    classCodeRepeater.itemAt(index).checked = false
                                }
                                includeSelectedMode.checked = false
                                excludeSelectedMode.checked = false
                                sampleModel.clearValueFilter()
                            }
                        }
                    }
                }

                GroupBox {
                    width: parent.width
                    title: "Return filter"

                    Column {
                        width: parent.width
                        spacing: 4

                        Repeater {
                            id: returnTypeRepeater
                            model: [
                                { type: 0, label: "First of many" },
                                { type: 1, label: "Last" },
                                { type: 2, label: "Last of many" },
                                { type: 3, label: "Single" }
                            ]

                            delegate: CheckBox {
                                required property var modelData
                                text: modelData.label
                                onToggled: {
                                    sampleModel.toggleReturnType(modelData.type)
                                    sampleModel.applyReturnFilter()
                                }
                            }
                        }

                        Button {
                            width: parent.width
                            text: "Clear return filter"
                            onClicked: {
                                for (let index = 0; index < returnTypeRepeater.count; ++index) {
                                    returnTypeRepeater.itemAt(index).checked = false
                                }
                                sampleModel.clearReturnFilter()
                            }
                        }
                    }
                }

                GroupBox {
                    width: parent.width
                    title: "Bitfield filter"

                    Column {
                        width: parent.width
                        spacing: 8

                        Row {
                            width: parent.width
                            spacing: 8

                            Column {
                                id: setBitsColumn
                                width: 64
                                spacing: 4

                                Label {
                                    width: parent.width
                                    text: "Set bits"
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                CheckBox {
                                    id: requiredSetScanDirectionBit
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    onToggled: {
                                        if (checked) {
                                            requiredClearScanDirectionBit.checked = false
                                        }
                                        sampleModel.toggleBitfieldBit(6, true)
                                        sampleModel.applyBitfieldFilter()
                                    }
                                }
                            }

                            Column {
                                width: 72
                                spacing: 4

                                Label {
                                    width: parent.width
                                    text: "Clear bits"
                                    font.weight: Font.DemiBold
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                CheckBox {
                                    id: requiredClearScanDirectionBit
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    onToggled: {
                                        if (checked) {
                                            requiredSetScanDirectionBit.checked = false
                                        }
                                        sampleModel.toggleBitfieldBit(6, false)
                                        sampleModel.applyBitfieldFilter()
                                    }
                                }
                            }

                            Item {
                                width: parent.width - 152
                                height: setBitsColumn.height

                                Label {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    height: requiredClearScanDirectionBit.height
                                    text: "Scan direction flag"
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }

                        Button {
                            width: parent.width
                            text: "Clear bitfield filter"
                            onClicked: {
                                requiredSetScanDirectionBit.checked = false
                                requiredClearScanDirectionBit.checked = false
                                sampleModel.clearBitfieldFilter()
                            }
                        }
                    }
                }

            }
        }
    }
}
