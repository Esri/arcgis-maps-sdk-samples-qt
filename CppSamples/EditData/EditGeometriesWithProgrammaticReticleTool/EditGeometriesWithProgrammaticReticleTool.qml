// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
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

import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Item {
    MapView {
        id: mapView
        anchors.fill: parent
        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Settings
    Item {
        width: 180
        height: 250
        anchors {
            top: parent.top
            right: parent.right
            margins: 10
        }

        ColumnLayout {
            spacing: 5
            anchors.fill: parent

            // Spacer
            Item {
                height: 5
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                Rectangle {
                    color: "#E0E0E0"
                    height: 60
                    width: 150
                    ColumnLayout {
                        anchors {
                            fill: parent
                            margins: 6
                        }
                        enabled: !reticleModel.geometryEditorStarted
                        Label {
                            text: qsTr("Geometry Type:")
                            color: "black"
                            font.pixelSize: 12
                            Layout.alignment: Qt.AlignVCenter
                        }
                        ComboBox {
                            id: geometryTypePicker
                            Layout.preferredWidth: 140
                            model: ["Point", "Multipoint", "Polygon", "Polyline"]
                            onCurrentIndexChanged: reticleModel.selectedGeometryType = currentIndex
                            background: Rectangle {
                                color: "white"
                                border.color: "#888"
                                border.width: 1
                            }
                            contentItem: Text {
                                color: "black"
                                text: geometryTypePicker.displayText
                                horizontalAlignment: Qt.AlignHCenter
                                verticalAlignment: Qt.AlignVCenter
                            }
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignCenter
                Rectangle {
                    color: "#E0E0E0"
                    height: 45
                    width: 150
                    // Vertex creation switch
                    RowLayout {
                        anchors {
                            fill: parent
                            margins: 6
                        }
                        Layout.alignment: Qt.AlignCenter
                        Label {
                            text: qsTr("Allow Vertex\n Creation:")
                            color: "black"
                            font.pixelSize: 12
                        }
                        Switch {
                            id: vertexSwitch
                            width: 40
                            height: 20
                            checked: reticleModel.vertexCreationAllowed
                            onCheckedChanged: reticleModel.vertexCreationAllowed = checked
                        }
                    }
                }
            }

            GridLayout {
                columns: 3
                rows: 2
                rowSpacing: 5
                columnSpacing: 5
                Layout.alignment: Qt.AlignCenter
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/undo-32.png"
                    tooltipText: qsTr("Undo")
                    enabled: reticleModel.canUndo
                    onClickedHandler: function() { reticleModel.undoOrCancel(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/redo-32.png"
                    tooltipText: qsTr("Redo")
                    enabled: reticleModel.canRedo
                    onClickedHandler: function() { reticleModel.redo(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/erase-32.png"
                    tooltipText: qsTr("Delete selected")
                    enabled: reticleModel.selectedElementCanDelete
                    onClickedHandler: function() { reticleModel.deleteSelectedElement(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/save-32.png"
                    tooltipText: qsTr("Save edits")
                    enabled: reticleModel.canUndo
                    onClickedHandler: function() {reticleModel.saveEdits();}
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/trash-32.png"
                    tooltipText: qsTr("Discard edits")
                    enabled: reticleModel.geometryEditorStarted
                    onClickedHandler: function() {reticleModel.discardEdits();}
                }
            }

            Rectangle {
                height: 35
                width: 150
                Layout.alignment: Qt.AlignCenter
                opacity: reticleModel.multifunctionButtonEnabled ? 1.0 : 0.5
                color : reticleModel.multifunctionButtonEnabled ? "white" : "#FF888888"
                Text {
                    id: multifunctionButtonText
                    anchors.fill: parent
                    text: reticleModel.multifunctionButtonText
                    color: "black"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                MouseArea {
                    anchors.fill: parent
                    enabled: reticleModel.multifunctionButtonEnabled
                    onClicked: reticleModel.handleMultifunctionButton()
                    cursorShape: Qt.PointingHandCursor
                }
            }
            // Spacer
            Item {
                height: 5
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    EditGeometriesWithProgrammaticReticleToolSample {
        id: reticleModel
        mapView: mapView
    }
}
