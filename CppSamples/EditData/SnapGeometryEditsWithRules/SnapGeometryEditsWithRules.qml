// [WriteFile Name=SnapGeometryEditsWithRules, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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
    // MapView component
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            forceActiveFocus(); // Enable keyboard navigation
        }
    }

    // Declare the C++ instance and bind the mapView
    SnapGeometryEditsWithRulesSample {
        id: snapGeometryEditsWithRulesModel
        mapView: mapView
    }

    Rectangle {
        id: instructionsPanel
        visible: snapGeometryEditsWithRulesModel.isElementSelected ? false : true
        anchors {
            right: parent.right
            margins: 10
        }
        width: textItem.width + 20
        height: textItem.height + 10
        color: palette.base

        Label {
            id: textItem
            anchors.centerIn: parent
            text: qsTr("Tap a point feature to edit.")
            font.pixelSize: 18
            font.bold: true
        }
    }

    Control {
        id: control
        visible: snapGeometryEditsWithRulesModel.isElementSelected ? true : false
        anchors.right: parent.right
        padding: 5

        background: Rectangle {
            color: palette.base
        }
        contentItem: ColumnLayout {
            id: columns
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            GridLayout {
                id: geometryColumn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Label {
                    id: geometryHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("Feature selected")
                    font.pixelSize: 18
                    font.bold: true
                }

                Label {
                    id: assetGroupText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("AssetGroup: ")
                    font.pixelSize: 14
                }

                Label {
                    id: assetTypeText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("AssetType: ")
                    font.pixelSize: 14
                }

                Connections {
                    target: snapGeometryEditsWithRulesModel
                    function onAssetGroupChanged(assetGroup) {
                        assetGroupText.text = "AssetGroup: " + assetGroup;
                    }
                    function onAssetTypeChanged(assetType) {
                        assetTypeText.text = "AssetType: " + assetType;
                    }
                }
            }

            GridLayout {
                id: editingColumn
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                GeometryEditorButton {
                    id: startButton
                    buttonName: qsTr("Start editor")
                    iconPath: ""
                    Layout.columnSpan: 2
                    enabled: !snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.startEditor();
                }
                GeometryEditorButton {
                    id: snapSettingsButton
                    buttonName: qsTr("Snap Settings")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/settings.png"
                    Layout.columnSpan: 2
                    enabled: snapGeometryEditsWithRulesModel.isElementSelected
                    onClicked: {
                        optionPanel.visible = !optionPanel.visible
                    }
                }
                GeometryEditorButton {
                    id: saveButton
                    buttonName: qsTr("Save")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/save-32.png"
                    enabled: snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.stopEditing();
                }

                GeometryEditorButton {
                    id: discardButton
                    buttonName: qsTr("Discard")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEditsWithRules/iconAssets/trash-32.png"
                    enabled: snapGeometryEditsWithRulesModel.geometryEditorStarted
                    onClicked: snapGeometryEditsWithRulesModel.discardEdits();
                }
            }

            GridLayout {
                id: snappingLegend
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Rectangle {
                    width: 10
                    height: 10
                    color: "green"
                    radius: 2
                }
                Label {
                    text: qsTr("None")
                    font.pixelSize: 14
                }

                Rectangle {
                    width: 10
                    height: 10
                    color: "orange"
                    radius: 2
                }
                Label {
                    text: qsTr("RulesLimitSnapping")
                    font.pixelSize: 14
                }

                Rectangle {
                    width: 10
                    height: 10
                    color: "red"
                    radius: 2
                }
                Label {
                    text: qsTr("RulesPreventSnapping")
                    font.pixelSize: 14
                }
            }
        }
    }

    Rectangle {
        id: optionPanel
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: 300
        visible: false
        color: palette.base

        Connections {
            target: snapGeometryEditsWithRulesModel
            function onIsElementSelectedChanged() {
                if (!snapGeometryEditsWithRulesModel.isElementSelected) {
                    optionPanel.visible = false;
                }
            }
        }

        ListView {
            id: snapSourceView
            anchors {
                fill: parent
                margins: 10
            }

            header: ColumnLayout {
                id: snappingColumn
                width: snapSourceView.width
                spacing: 0

                RowLayout {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 35
                    spacing: 10

                    Label {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        text: qsTr("Snapping")
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }

            model: snapGeometryEditsWithRulesModel.snapSourcesListModel

            delegate: Item {
                height: 35
                width: snapSourceView.width

                Rectangle {
                    id: wrapper
                    color: palette.mid
                    width: parent.width
                    height: parent.height

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 10

                        Label {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.fillWidth: true
                            text: name
                            font.pixelSize: 14
                            elide: Text.ElideRight
                        }

                        Switch {
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            checked: isEnabled
                            onCheckedChanged: {
                                if (isEnabled !== checked) {
                                    isEnabled = checked;
                                }
                            }
                        }
                    }
                }
            }

            footer: Item {
                width: snapSourceView.width
                height: 50

                Button {
                    text: qsTr("Done")
                    anchors.centerIn: parent
                    anchors.topMargin: 10
                    font.pixelSize: 14
                    font.bold: true

                    onClicked: {
                        optionPanel.visible = false
                    }
                }
            }
        }
    }
}
