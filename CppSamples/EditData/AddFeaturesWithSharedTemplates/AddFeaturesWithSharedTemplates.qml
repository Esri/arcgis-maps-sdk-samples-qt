// [WriteFile Name=AddFeaturesWithSharedTemplates, Category=EditData]
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
import QtQuick.Layouts
import Esri.Samples

Item {
    id: root

    readonly property bool editing: sampleModel.hasPendingEdits
    readonly property bool drawing: sampleModel.drawing
    readonly property bool hasTemplates: sampleModel.templates.length > 0

    component ToolbarButton: Button {
        Layout.minimumHeight: 44
        focusPolicy: Qt.TabFocus
    }

    MapView {
        id: mapView
        anchors.fill: parent
        Component.onCompleted: forceActiveFocus()
    }

    AddFeaturesWithSharedTemplatesSample {
        id: sampleModel
        mapView: mapView
        onErrorOccurred: function (message) {
            errorMessage.text = message;
            errorDialog.open();
        }
    }

    Pane {
        id: statusPanel
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        padding: 8
        visible: !noTemplatesPanel.visible

        RowLayout {
            width: parent.width
            ColumnLayout {
                Layout.fillWidth: true
                Label {
                    text: sampleModel.statusText
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
                Label {
                    visible: !sampleModel.busy && !root.drawing && root.hasTemplates && text !== sampleModel.statusText
                    text: root.editing ? qsTr("Save or undo edits.") : qsTr("Open Shared Templates and select a template to create features.")
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                }
            }
            BusyIndicator {
                running: sampleModel.busy
                visible: running
                Layout.preferredWidth: 32
                Layout.preferredHeight: 32
            }
        }
    }

    Pane {
        id: noTemplatesPanel
        anchors.centerIn: parent
        width: Math.min(parent.width - 32, 360)
        visible: sampleModel.ready && !root.hasTemplates
        ColumnLayout {
            width: parent.width
            Label {
                text: qsTr("No Templates")
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
            Label {
                text: qsTr("This map has no preset or group shared templates.")
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }

    ToolBar {
        id: toolbar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        topPadding: 12
        bottomPadding: 12
        leftPadding: 12
        rightPadding: 12
        visible: root.hasTemplates
        contentItem: RowLayout {
            spacing: 12
            enabled: !sampleModel.busy
            ToolbarButton {
                text: root.editing ? qsTr("Save") : qsTr("Cancel")
                highlighted: root.editing
                visible: root.editing || root.drawing
                onClicked: root.editing
                           ? sampleModel.saveEdits()
                           : sampleModel.cancelDrawing()
            }
            Item {
                Layout.fillWidth: true
            }
            ToolbarButton {
                text: qsTr("Shared Templates")
                highlighted: true
                visible: !root.editing && !root.drawing
                onClicked: templatePicker.open()
            }
            Item {
                Layout.fillWidth: true
            }
            ToolbarButton {
                text: root.editing ? qsTr("Undo") : qsTr("Complete")
                highlighted: !root.editing
                visible: root.editing || root.drawing
                enabled: root.editing || sampleModel.canComplete
                onClicked: root.editing
                           ? sampleModel.undoEdits()
                           : sampleModel.completeDrawing()
            }
        }
    }

    Popup {
        id: templatePicker
        parent: root
        x: (parent.width - width) / 2
        y: Math.max(8, toolbar.y - height - 8)
        width: Math.min(320, parent.width - 16)
        height: Math.min(templateContent.implicitHeight + topPadding + bottomPadding, Math.max(0, toolbar.y - statusPanel.height - 16))
        padding: 12
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        contentItem: ScrollView {
            id: templateScroll
            clip: true
            contentWidth: availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                id: templateContent
                width: templateScroll.availableWidth
                spacing: 8
                Label {
                    text: qsTr("Shared Templates")
                    font.bold: true
                    Layout.fillWidth: true
                }
                Repeater {
                    model: sampleModel.templates
                    delegate: ItemDelegate {
                        id: templateDelegate
                        required property int index
                        required property var modelData
                        Layout.fillWidth: true
                        enabled: !sampleModel.busy
                        hoverEnabled: true
                        focusPolicy: Qt.TabFocus
                        Accessible.name: modelData.name
                        Accessible.description: modelData.description
                        ToolTip.visible: hovered || pressed || visualFocus
                        ToolTip.text: modelData.description
                        ToolTip.delay: 500
                        contentItem: RowLayout {
                            spacing: 8
                            Item {
                                Layout.preferredWidth: 36
                                Layout.preferredHeight: 36
                                Image {
                                    id: templateSwatch
                                    anchors.fill: parent
                                    source: templateDelegate.modelData.swatch
                                    fillMode: Image.PreserveAspectFit
                                    visible: status === Image.Ready
                                }
                                Label {
                                    anchors.centerIn: parent
                                    text: "+"
                                    font.pixelSize: 28
                                    visible: templateSwatch.status !== Image.Ready
                                    Accessible.ignored: true
                                }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                Label {
                                    text: templateDelegate.modelData.name
                                    font.bold: true
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                }
                                Label {
                                    text: templateDelegate.modelData.kind
                                    font.pixelSize: 12
                                }
                            }
                        }
                        onClicked: {
                            sampleModel.startDrawing(index);
                            if (sampleModel.drawing) {
                                templatePicker.close();
                            }
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: errorDialog
        parent: root
        anchors.centerIn: parent
        width: Math.min(parent.width - 32, 420)
        height: Math.min(implicitHeight, Math.max(0, parent.height - 32))
        title: qsTr("Error")
        modal: true
        standardButtons: Dialog.Ok
        contentItem: ScrollView {
            id: errorScroll
            clip: true
            contentWidth: availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            Label {
                id: errorMessage
                width: errorScroll.availableWidth
                wrapMode: Text.WordWrap
            }
        }
    }
}
