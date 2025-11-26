// [WriteFile Name=FilterBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.

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
import Esri.ArcGISRuntime.Toolkit

Item {
    LocalSceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: forceActiveFocus()
    }

    FilterBuildingSceneLayerSample {
        id: sampleModel
        localSceneView: view

        onPopupChanged: popupView.visible = true
    }

    Button {
        anchors {
            top: view.top
            right: view.right
            margins: 15
        }
        text: qsTr("Building Filter Settings")
        onClicked: settingsPanel.visible = true
    }

    Rectangle {
        id: settingsPanel
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: 300
        visible: false
        color: palette.base
        opacity: 1

        MouseArea {
            anchors.fill: settingsPanel
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 14
            Layout.margins: 16

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: qsTr("Settings")
                    font.pixelSize: 18
                    font.bold: true
                    padding: 6
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Done")
                    font.pixelSize: 18
                    font.bold: true
                    padding: 6
                    horizontalAlignment: Text.AlignRight

                    MouseArea {
                        anchors.fill: parent
                        onClicked: settingsPanel.visible = false
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Layout.leftMargin: 12
                Layout.rightMargin: 12

                Label {
                    text: qsTr("Floor:")
                    font.pixelSize: 16
                    Layout.alignment: Qt.AlignVCenter
                }

                Item { Layout.fillWidth: true }

                ComboBox {
                    id: floorSelector
                    model: sampleModel.floorList

                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 28

                    font.pixelSize: 14
                    Component.onCompleted: floorSelector.currentIndex = 0
                    onCurrentTextChanged: sampleModel.updateFloorFilter(currentText)
                }
            }

            Label {
                text: qsTr("Categories")
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 18
                clip: true
                model: sampleModel.sublayerListModel

                delegate: Item {
                    width: settingsPanel.width
                    height: container.implicitHeight

                    ColumnLayout {
                        id: container
                        width: parent.width
                        spacing: 10

                        Item {
                            width: parent.width
                            height: category.implicitHeight

                            Label {
                                id: category
                                text: name
                                font.pixelSize: 15
                                font.bold: true
                                anchors.left: parent.left
                                anchors.leftMargin: 12
                            }

                            CheckBox {
                                id: categoryCheckbox
                                checked: model.visible
                                onCheckedChanged: {
                                    if (model.visible !== checked) {
                                        model.visible = checked
                                    }
                                }
                                anchors.right: parent.right
                                anchors.rightMargin: 12
                            }
                        }

                        Repeater {
                            model: sampleModel.getComponentSubLayerListModel(index)

                            Item {
                                width: parent.width
                                height: sublayer.implicitHeight

                                Label {
                                    id: sublayer
                                    text: name
                                    font.pixelSize: 13
                                    anchors.left: parent.left
                                    anchors.leftMargin: 32
                                }

                                CheckBox {
                                    checked: model.visible
                                    onCheckedChanged: {
                                        if (model.visible !== checked) {
                                            model.visible = checked
                                        }
                                    }
                                    anchors.right: parent.right
                                    anchors.rightMargin: 12
                                    enabled: categoryCheckbox.checked
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    PopupView {
        id: popupView
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        popup: sampleModel.popup

        visible: false

        onVisibleChanged: {
            if (!visible)   {
                sampleModel.clearSelection();
            }
        }
    }

}
