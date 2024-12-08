// [WriteFile Name=SnapGeometryEdits, Category=EditData]
// [Legal]
// Copyright 2024 Esri.
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

        onGeometryEditorStartedChanged: {
            if (!geometryEditorStarted) {
                pointButton.checked = false;
                multiPointButton.checked = false;
                lineButton.checked = false;
                polygonButton.checked = false;
            }
        }
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
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: 360
        visible: false
        color: "white"
        opacity: 1

        ListView {
            id: snapSourceView
            anchors {
                fill: parent
                margins: 10
            }

            header: ColumnLayout {
                id: snappingColumn
                Layout.minimumWidth: optionPanel.width
                spacing: 0
                RowLayout {
                    Layout.minimumWidth: optionPanel.width
                    Layout.minimumHeight: 35

                    Text {
                        Layout.alignment: Qt.AlignLeft
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width * 0.75
                        text: "Snapping"
                        font.pixelSize: 15
                        color: "#8434C1"
                        font.bold: true
                    }

                    Text {
                        Layout.alignment: Qt.AlignRight
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width * 0.25
                        text: "Done"
                        font.pixelSize: 15
                        color: "#8434C1"
                        font.bold: true
                        MouseArea {
                            anchors.fill: parent
                            onClicked: optionPanel.visible = false;
                        }
                    }
                }

                ColumnLayout {
                    Layout.minimumWidth: optionPanel.width
                    Layout.minimumHeight: 35
                    spacing: 0
                    Rectangle {
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: snapSourceView.width - (snapSourceView.anchors.margins / 2)
                        Layout.minimumHeight: 35
                        color: "#E9DFEA"

                        Text {
                            text: qsTr("Snapping enabled")
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
                    Rectangle {
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: snapSourceView.width - (snapSourceView.anchors.margins / 2)
                        Layout.minimumHeight: 35
                        color: "#E9DFEA"

                        Text {
                            text: qsTr("Geometry guides")
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
                            onCheckedChanged: snapGeometryEditsSampleModel.geometryGuidesEnabledStatus(checked)
                        }
                    }
                    Rectangle {
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: snapSourceView.width - (snapSourceView.anchors.margins / 2)
                        Layout.minimumHeight: 35
                        color: "#E9DFEA"


                        Text {
                            text: qsTr("Feature snapping")
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
                            onCheckedChanged: snapGeometryEditsSampleModel.featureSnappingEnabledStatus(checked)
                        }
                    }
                }
            }

            model: snapGeometryEditsSampleModel.snapSourceModel

            section {
                property: "section"
                criteria: ViewSection.FullString
                delegate: Item {
                    height: 35
                    width: optionPanel.width

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width
                        Layout.minimumHeight: 25

                        Text {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            Layout.minimumWidth: optionPanel.width * 0.5
                            Layout.minimumHeight: 25
                            color: "#8434C1"
                            text: section + " layers"
                            font.pixelSize: 15
                            font.bold: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                        }

                        Text {
                            Layout.alignment: Qt.AlignRight
                            Layout.minimumWidth: optionPanel.width * 0.5
                            Layout.minimumHeight: 25
                            text: "Enable All Sources"
                            font.pixelSize: 15
                            color: "#8434C1"
                            font.bold: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10

                            MouseArea {
                                anchors.fill: parent
                                onClicked: snapGeometryEditsSampleModel.enableAllLayersInSection(section);
                            }
                        }
                    }
                }
            }

            delegate: Item {
                height: 35
                width: optionPanel.width
                id:delegate
                Rectangle {
                    id: wrapper
                    color: "#E9DFEA"
                    width: snapSourceView.width - (snapSourceView.anchors.margins / 2)
                    height: delegate.height
                    anchors {
                        margins: 15
                    }

                    RowLayout {
                        id : row
                        Layout.fillWidth: true
                        Layout.minimumWidth: optionPanel.width
                        width: wrapper.width

                        Text {
                            Layout.alignment: Qt.AlignLeft
                            Layout.fillWidth: true
                            Layout.minimumWidth: optionPanel.width / 2
                            text: name
                            font.pixelSize: 15
                            Layout.leftMargin: 10
                        }

                        Switch {
                            Layout.alignment: Qt.AlignRight
                            Layout.fillWidth: true
                            checked: isEnabled
                            onCheckedChanged: isEnabled = checked;
                        }
                    }
                }
            }
        }
    }
}
