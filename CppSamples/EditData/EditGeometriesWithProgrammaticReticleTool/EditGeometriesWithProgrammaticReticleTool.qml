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
    Rectangle {
        anchors.margins: 10
        width: 180
        height: 240
        anchors.top: parent.top
        anchors.right: parent.right
        color: "#AA333333"
        border.color: "#888"
        border.width: 1

        ColumnLayout {
            spacing: 5
            anchors.fill: parent

            // Spacer
            Item {
                height: 5
            }

            RowLayout {
                enabled: !reticleModel.geometryEditorStarted
                Layout.alignment: Qt.AlignCenter
                Label {
                    text: "Geometry\nType:"
                    color: "white"
                }
                ComboBox {
                    id: geometryTypePicker
                    model: ["Point", "Multipoint", "Polygon", "Polyline"]
                    onCurrentIndexChanged: reticleModel.selectedGeometryType = currentIndex
                    background: Rectangle {
                        color: "white"
                    }
                    contentItem: Text {
                        color: "black"
                        text: geometryTypePicker.displayText
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }

            // Vertex creation switch
            RowLayout {
                spacing: 5
                Layout.alignment: Qt.AlignCenter
                Label {
                    text: "Allow Vertex\nCreation:"
                    color: "white"
                }
                Switch {
                    id: vertexSwitch
                    checked: reticleModel.vertexCreationAllowed
                    onCheckedChanged: reticleModel.vertexCreationAllowed = checked
                }
            }

            GridLayout {
                columns: 3
                rows: 2
                rowSpacing: 10
                columnSpacing: 20
                Layout.alignment: Qt.AlignCenter
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/undo-32.png"
                    tooltipText: "Undo"
                    buttonOpacity: reticleModel.canUndo ? 1.0 : 0.5
                    enabled: reticleModel.canUndo
                    onClickedHandler: function() { reticleModel.undoOrCancel(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/redo-32.png"
                    tooltipText: "Redo"
                    buttonOpacity: reticleModel.canRedo ? 1.0 : 0.5
                    enabled: reticleModel.canRedo
                    onClickedHandler: function() { reticleModel.redo(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/erase-32.png"
                    tooltipText: "Delete selected"
                    buttonOpacity: reticleModel.selectedElementCanDelete ? 1.0 : 0.5
                    enabled: reticleModel.selectedElementCanDelete
                    onClickedHandler: function() { reticleModel.deleteSelectedElement(); }
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/save-32.png"
                    tooltipText: "Save edits"
                    buttonOpacity: reticleModel.canUndo ? 1.0 : 0.5
                    enabled: reticleModel.canUndo
                    onClickedHandler: function() {reticleModel.saveEdits();}
                }
                GeometryEditorButton {
                    iconSource: "qrc:/Samples/EditData/EditGeometriesWithProgrammaticReticleTool/iconAssets/trash-32.png"
                    tooltipText: "Discard edits"
                    buttonOpacity: reticleModel.geometryEditorStarted ? 1.0 : 0.5
                    enabled: reticleModel.geometryEditorStarted
                    onClickedHandler: function() {reticleModel.discardEdits();}
                }
            }

            Rectangle {
                height: 30
                width: 150
                Layout.alignment: Qt.AlignCenter
                opacity: reticleModel.multifunctionButtonEnabled ? 1.0 : 0.5
                color : reticleModel.multifunctionButtonEnabled ? "white" : "#FF888888"
                Text {
                    id: multifunctionButtonText
                    anchors.fill: parent
                    text: reticleModel.multifunctionButtonText
                    color: "black"
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
