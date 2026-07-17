// [WriteFile Name=DisplayGeometryEditorInformationDuringInteraction, Category=EditData]
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
import QtQuick.Controls.Basic as Basic
import QtQuick.Layouts
import Esri.Samples

Item {
    id: root

    MapView {
        id: mapView
        anchors.fill: root

        Component.onCompleted: forceActiveFocus()
    }

    DisplayGeometryEditorInformationDuringInteractionSample {
        id: sampleModel
        mapView: mapView
    }

    component AccentButton: Basic.Button {
        id: accentButton

        display: AbstractButton.IconOnly
        icon.width: 32
        icon.height: 32
        icon.color: "#ffffff"
        opacity: enabled ? 1 : 0.3

        background: Rectangle {
            implicitHeight: 48
            color: accentButton.down ? Qt.darker(accentButton.palette.highlight, 1.2) :
                                       accentButton.hovered ? Qt.lighter(accentButton.palette.highlight, 1.1) :
                                                              accentButton.palette.highlight
        }
    }

    Control {
        readonly property real maximumWidth: Math.max(0, Math.min(380, root.width - 20))

        width: sampleModel.geometryEditorStarted ? maximumWidth :
                                                   Math.min(maximumWidth, promptLabel.implicitWidth + leftPadding + rightPadding)
        padding: 10

        anchors {
            top: root.top
            right: root.right
            margins: 10
        }

        background: Rectangle {
            color: palette.base
            opacity: 0.9

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: mouse => mouse.accepted = true
                onDoubleClicked: mouse => mouse.accepted = true
                onWheel: wheel => wheel.accepted = true
            }
        }

        contentItem: ColumnLayout {
            spacing: 8

            Label {
                id: promptLabel

                visible: !sampleModel.geometryEditorStarted
                text: qsTr("Tap a graphic to start the geometry editor.")
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            GridLayout {
                columns: 2
                visible: sampleModel.geometryEditorStarted

                AccentButton {
                    text: qsTr("Save edits")
                    icon.source: "qrc:/Samples/EditData/DisplayGeometryEditorInformationDuringInteraction/check-circle-24.svg"

                    Layout.fillWidth: true

                    ToolTip.visible: hovered
                    ToolTip.text: text

                    onClicked: sampleModel.saveEdits()
                }

                AccentButton {
                    text: qsTr("Discard edits")
                    icon.source: "qrc:/Samples/EditData/DisplayGeometryEditorInformationDuringInteraction/circle-disallowed-24.svg"

                    Layout.fillWidth: true

                    ToolTip.visible: hovered
                    ToolTip.text: text

                    onClicked: sampleModel.cancelEdits()
                }

                AccentButton {
                    text: qsTr("Undo")
                    enabled: sampleModel.canUndo
                    icon.source: "qrc:/Samples/EditData/DisplayGeometryEditorInformationDuringInteraction/undo-24.svg"

                    Layout.fillWidth: true

                    ToolTip.visible: hovered
                    ToolTip.text: text

                    onClicked: sampleModel.undo()
                }

                AccentButton {
                    text: qsTr("Redo")
                    enabled: sampleModel.canRedo
                    icon.source: "qrc:/Samples/EditData/DisplayGeometryEditorInformationDuringInteraction/redo-24.svg"

                    Layout.fillWidth: true

                    ToolTip.visible: hovered
                    ToolTip.text: text

                    onClicked: sampleModel.redo()
                }
            }

            RowLayout {
                visible: sampleModel.interactionInfoVisible

                Layout.fillWidth: true

                Label {
                    text: sampleModel.interactionDescription
                    wrapMode: Text.WordWrap
                }

                Label {
                    text: sampleModel.interactionValue
                    // ensure the size of the text displayed doesn't
                    // judder as the numbers rapidly change on interacting
                    font.features: ({ "tnum": 1 })
                }
            }
        }
    }
}
