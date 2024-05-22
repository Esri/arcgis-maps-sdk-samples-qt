// [WriteFile Name=SnapGeometryEdits, Category=EditData]
// [Legal]
// Copyright 2024 Esri.

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
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    SnapGeometryEditsSample {
        id: snapGeometryEditsSampleModel
        mapView: view
    }

    Rectangle {
        anchors {
            right: parent.right
            margins: 10
        }

        Control {
            id: control
            anchors.right: parent.right
            padding: 5
            width: 140

            background: Rectangle {
                color: "black"
                opacity: .5
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

                    Text {
                        id: geometryHeader
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.columnSpan: 2
                        text: "Create"
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    GeometryEditorButton {
                        id: pointButton
                        buttonName: qsTr("Point")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/point-32.png"
                        checkable: true
                        enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                        onClicked: snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PointMode);
                    }

                    GeometryEditorButton {
                        id: multiPointButton
                        buttonName: qsTr("Multipoint")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/multipoint-32.png"
                        checkable: true
                        enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                        onClicked: snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.MultipointMode);
                    }

                    GeometryEditorButton {
                        id: lineButton
                        buttonName: qsTr("Line")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/line-32.png"
                        checkable: true
                        enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                        onClicked: snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PolylineMode);
                    }

                    GeometryEditorButton {
                        id: polygonButton
                        buttonName: qsTr("Polygon")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/polygon-32.png"
                        checkable: true
                        enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                        onClicked: snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PolygonMode);
                    }
                }

                GridLayout {
                    id: editingColumn
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    columns: 2

                    Text {
                        id: editingHeader
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.columnSpan: 2
                        text: qsTr("Edit")
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    GeometryEditorButton {
                        id: undoButton
                        buttonName: qsTr("Undo")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/undo-32.png"
                        enabled: snapGeometryEditsSampleModel.geometryEditorStarted && snapGeometryEditsSampleModel.canUndo
                        onClicked: snapGeometryEditsSampleModel.editorUndo();
                    }

                    GeometryEditorButton {
                        id: deleteButton
                        buttonName: qsTr("Delete")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/erase-32.png"
                        enabled: snapGeometryEditsSampleModel.geometryEditorStarted || snapGeometryEditsSampleModel.isElementSelected
                        onClicked: snapGeometryEditsSampleModel.deleteSelection();
                    }

                    GeometryEditorButton {
                        id: saveEditsButton
                        buttonName: qsTr("Save")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/save-32.png"
                        Layout.columnSpan: 2
                        enabled: snapGeometryEditsSampleModel.geometryEditorStarted
                        onClicked: snapGeometryEditsSampleModel.stopEditing();
                    }

                    GeometryEditorButton {
                        id: snapSettingsButton
                        buttonName: qsTr("Snap Settings")
                        iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/settings.png"
                        Layout.columnSpan: 2
                        enabled: snapGeometryEditsSampleModel.layersLoaded
                        onClicked: {
                            optionPanel.visible = true;
                            snapGeometryEditsSampleModel.displaySnapSources();
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: optionPanel
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: 360
        visible: false
        color: "white"
        opacity: 1

        ListView {
            id: snapSourceView
            width: optionPanel.width
            height: optionPanel.height
            headerPositioning: ListView.OverlayHeader
            spacing: 20

            header: Column {
                id: snappingColumn
                width: optionPanel.width
                height: 100

                Item {
                    width: optionPanel.width
                    height: 25

                    Text {
                        text: "Snapping"
                        font.pixelSize: 16
                        color: "#8434C1"
                        font.bold: true

                    }

                    Rectangle {
                        anchors {
                            right: parent.right
                            margins: 10
                        }
                        width: 45
                        height: 20

                        Text {
                            text: "Done"
                            font.pixelSize: 16
                            color: "#8434C1"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: optionPanel.visible = false;
                        }
                    }
                }

                Item {
                    width: optionPanel.width
                    height: 25

                    Rectangle {
                        anchors {
                            margins: 10
                        }

                        width: 340
                        height: 45
                        color: "#E9DFEA"
                        radius: 5
                        border {
                            color: "#4D4D4D"
                            width: 1
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Enabled")
                            font.pixelSize: 15
                            anchors {
                                left: parent.left
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                        }

                        Switch {
                            anchors {
                                right: parent.right
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            onCheckedChanged: snapGeometryEditsSampleModel.snappingEnabledStatus(checked)
                        }
                    }
                }
            }


            model: snapGeometryEditsSampleModel.snapSourceModel

            section {
                property: "section"
                criteria: ViewSection.FullString
                delegate: Item {
                    height: 25
                    width: optionPanel.width

                    RowLayout {
                        Layout.margins: 10
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width

                        Text {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            text: section
                            font.pixelSize: 16
                            font.bold: true
                        }

                        Rectangle {
                            Layout.alignment: Qt.AlignRight
                            width: 140
                            height: 20

                            Text {
                                text: "Enable All Sources"
                                font.pixelSize: 15
                                color: "#8434C1"
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: snapGeometryEditsSampleModel.enableAllLayersInSection(section, true);
                            }
                        }
                    }
                }
            }

            delegate: Item {
                height: 25
                width: optionPanel.width

                RowLayout {
                    Layout.margins: 10
                    Layout.fillWidth: true
                    Layout.minimumWidth: optionPanel.width

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        text: name
                        font.pixelSize: 12
                    }

                    Switch {
                        Layout.alignment: Qt.AlignRight
                        checked: isEnabled
                        onCheckedChanged: isEnabled = checked;
                    }
                }
            }
        }
    }
}
