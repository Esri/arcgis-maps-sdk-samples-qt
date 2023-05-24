// [WriteFile Name=CreateAndEditGeometries, Category=Geometry]
// [Legal]
// Copyright 2023 Esri.

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
    CreateAndEditGeometriesSample {
        id: model
        mapView: view

        onGeometryEditorStartedChanged: {
            // Uncheck UI buttons when the geometry editor stops
            if (!geometryEditorStarted) {
                pointButton.checked = false;
                multiPointButton.checked = false;
                lineButton.checked = false;
                polygonButton.checked = false;
            }
        }
    }

    // Prevent mouse interaction from propagating to the MapView
    MouseArea {
        anchors.fill: control
        onPressed: mouse => mouse.accepted = true;
        onWheel: wheel => wheel.accepted = true;
    }

    Control {
        id: control
        anchors.right: parent.right
        padding: 5
        width: 130

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
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/point-32.png"
                    checkable: true
                    enabled: !model.geometryEditorStarted
                    onClicked: {
                        toolCombo.currentIndex = 0;
                        toolCombo.enabled = false;
                        model.startGeometryEditorWithGeometryType(CreateAndEditGeometriesSample.PointMode);
                    }
                }

                GeometryEditorButton {
                    id: multiPointButton
                    buttonName: qsTr("Multipoint")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/multipoint-32.png"
                    checkable: true
                    enabled: !model.geometryEditorStarted
                    onClicked: {
                        toolCombo.currentIndex = 0;
                        toolCombo.enabled = false;
                        model.startGeometryEditorWithGeometryType(CreateAndEditGeometriesSample.MultipointMode);
                    }
                }

                GeometryEditorButton {
                    id: lineButton
                    buttonName: qsTr("Line")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/line-32.png"
                    checkable: true
                    enabled: !model.geometryEditorStarted
                    onClicked: {
                        model.startGeometryEditorWithGeometryType(CreateAndEditGeometriesSample.PolylineMode);
                    }
                }

                GeometryEditorButton {
                    id: polygonButton
                    buttonName: qsTr("Polygon")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/polygon-32.png"
                    checkable: true
                    enabled: !model.geometryEditorStarted
                    onClicked: model.startGeometryEditorWithGeometryType(CreateAndEditGeometriesSample.PolygonMode);
                }

                ComboBox {
                    id: toolCombo
                    model: [qsTr("VertexTool"), qsTr("FreehandTool")]
                    Layout.columnSpan: 2
                    Layout.fillWidth: true

                    Rectangle {
                        anchors.fill: parent
                        radius: 10
                        // Make the rectangle visible if a dropdown indicator exists
                        // An indicator only exists if a theme is set
                        visible: parent.indicator
                        border.width: 1
                    }

                    onCurrentIndexChanged: {
                        switch (currentIndex) {
                        case 0: // Vertex Tool
                            model.setTool(CreateAndEditGeometriesSample.Vertex);
                            break;
                        case 1: // Freehand Tool
                            model.setTool(CreateAndEditGeometriesSample.Freehand);
                            break;
                        default:
                            model.setTool(CreateAndEditGeometriesSample.Vertex);
                        }
                    }
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
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/undo-32.png"
                    enabled: model.geometryEditorStarted && model.canUndo
                    onClicked: model.undo();
                }

                GeometryEditorButton {
                    id: redoButton
                    buttonName: qsTr("Redo")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/redo-32.png"
                    enabled: model.geometryEditorStarted && model.canRedo
                    onClicked: model.redo();
                }

                GeometryEditorButton {
                    id: deleteVertexButton
                    buttonName: qsTr("Delete selected element")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/erase-32.png"
                    Layout.columnSpan: 2
                    enabled: model.geometryEditorStarted && model.elementIsSelected
                    onClicked: model.deleteSelectedElement();
                }

                GeometryEditorButton {
                    id: saveEditsButton
                    buttonName: qsTr("Stop and save edits")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/check-circle-32.png"
                    Layout.columnSpan: 2
                    enabled: model.geometryEditorStarted
                    onClicked: {
                        toolCombo.enabled = true;
                        model.stopEditing(true);
                    }
                }

                GeometryEditorButton {
                    id: discardEditsButton
                    buttonName: qsTr("Stop and discard edits")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/circle-disallowed-32.png"
                    Layout.columnSpan: 2
                    enabled: model.geometryEditorStarted
                    onClicked: {
                        toolCombo.enabled = true;
                        model.stopEditing(false);
                    }
                }

                GeometryEditorButton {
                    id: clearGraphicsButton
                    buttonName: qsTr("Delete all geometries")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/trash-32.png"
                    Layout.columnSpan: 2
                    enabled: !model.geometryEditorStarted
                    onClicked: model.clearGraphics();
                }
            }
        }
    }
}
