// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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

    enum DrawingModes {
        NotDrawing,
        Drawing,
        Point,
        Multipoint,
        Line,
        Polygon
    }

    property int drawStatus: GeometryEditorOnMap.DrawingModes.NotDrawing

    GeometryEditorSample {
        id: model
        mapView: view
    }

    MapView {
        id: view
        anchors.fill: parent

        // Force focus to remain on MapView so GeometryEditor will respond to keystrokes
        onFocusChanged: focus = true;

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Display an option to delete the selected vertex if the user right-clicks or taps and holds when the GeometryEditor is started
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        enabled: model.canDeleteSelection || model.canUndo || model.canRedo
        onClicked: {
            contextMenu.popup()
        }

        Menu {
            id: contextMenu
            width: actionComponent.width
            Action {
                enabled: model.canDeleteSelection
                id: deleteAction
                text: "Delete selection"
                onTriggered: model.deleteSelection();
            }

            Action {
                enabled: model.canUndo
                id: undoAction
                text: "Undo"
                onTriggered: model.undo();
            }

            Action {
                enabled: model.canRedo
                id: redoAction
                text: "Redo"
                onTriggered: model.redo();
            }
        }
    }

    Control {
        id: control
        anchors.right: parent.right
        padding: 5
        width: 200

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
            spacing: 20

            GridLayout {
                id: geometryColumn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                SketchEditorButton {
                    id: ptButton
                    buttonName: "Start Point"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/point-32.png"

                    highlighted: drawStatus === GeometryEditorOnMap.DrawingModes.Point

                    onClicked: {
                        model.setCreationMode(GeometryEditorSample.PointMode);
                        drawStatus = GeometryEditorOnMap.DrawingModes.Point;
                    }
                }

                SketchEditorButton {
                    id: mPtButton
                    buttonName: "Start Multi"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/point-32.png"
                    images: 2

                    highlighted: drawStatus === GeometryEditorOnMap.DrawingModes.Multipoint

                    onClicked: {
                        model.setCreationMode(GeometryEditorSample.MultipointMode);
                        drawStatus = GeometryEditorOnMap.DrawingModes.Multipoint;
                    }
                }

                SketchEditorButton {
                    id: lineButton
                    buttonName: "Start Polyline"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/line-32.png"

                    highlighted: drawStatus === GeometryEditorOnMap.DrawingModes.Line

                    onClicked: {
                        model.setCreationMode(GeometryEditorSample.PolylineMode);
                        drawStatus = GeometryEditorOnMap.DrawingModes.Line;
                    }
                }

                SketchEditorButton {
                    id: polygonButton
                    buttonName: "Start Polygon"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/polygon-32.png"

                    highlighted: drawStatus === GeometryEditorOnMap.DrawingModes.Polygon

                    onClicked: {
                        model.setCreationMode(GeometryEditorSample.PolygonMode);
                        drawStatus = GeometryEditorOnMap.DrawingModes.Polygon;
                    }
                }

                SketchEditorButton {
                    id: geometryStartButton
                    buttonName: "Start with highlighted geometry"
                    columnSpan: 2
                    highlighted: drawStatus === GeometryEditorOnMap.DrawingModes.Drawing

                    enabled: model.isGeoElementSelected
                    opacity: enabled ? 1 : 0.4

                    onClicked: {
                        model.startGeometry();
                        drawStatus = GeometryEditorOnMap.DrawingModes.Drawing;
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
                    text: "Editing"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                SketchEditorButton {
                    id: undoButton
                    buttonName: "Undo"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/undo-32.png"

                    enabled: model.canUndo
                    opacity: enabled ? 1 : 0.4

                    onClicked: model.undo();
                }

                SketchEditorButton {
                    id: redoButton
                    buttonName: "Redo"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/redo-32.png"

                    enabled: model.canRedo
                    opacity: enabled ? 1 : 0.4

                    onClicked: model.redo();
                }

                SketchEditorButton {
                    id: deleteSelectedVertexButton
                    columnSpan: 2
                    buttonName: "Delete selection"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/erase-32.png"

                    enabled: model.canDeleteSelection
                    opacity: enabled ? 1 : 0.4

                    onClicked: model.deleteSelection();
                }

                SketchEditorButton {
                    id: saveEditsButton
                    columnSpan: 2
                    buttonName: "Save and stop"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/check-circle-32.png"

                    enabled: model.geometryEditorStarted
                    opacity: enabled ? 1 : 0.4

                    onClicked: {
                        model.stopGeometryEditor(true);
                        drawStatus = GeometryEditorOnMap.DrawingModes.NotDrawing
                        if (!model.geometryEditorStarted && !clearGraphicsButton.enabled)
                            clearGraphicsButton.enabled = true;
                    }
                }

                SketchEditorButton {
                    id: clearGraphicsButton
                    columnSpan: 2
                    buttonName: "Clear geometry and graphics"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/trash-32.png"           

                    onClicked: {
                        model.clearGraphics();                  
                    }
                }
            }

            GridLayout {
                id: configColumn
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Text {
                    id: configHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "Configuration"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                Text {
                    Layout.alignment: Qt.AlignHRight | Qt.AlignVCenter
                    text: "<font color=\"white\">Select only</font>"
                }

                Switch {
                    checked: true
                    text: "<font color=\"white\">Interaction Edit</font>"
                    onCheckedChanged: {
                        model.toggleInteractionEdit(checked);
                    }
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: true
                    text: "<font color=\"white\">Allow Selection move</font>"
                    onCheckedChanged: {
                        model.toggleAllowSelectionMove(checked);
                    }
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: false
                    text: "<font color=\"white\">Require Selection before move</font>"
                    onCheckedChanged: {
                        model.toggleRequireSelection(checked);
                    }
                }

                Text {
                    Layout.columnSpan: 2
                    id: hackSpace
                    text: ""
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: true
                    text: "<font color=\"white\">Allow Vertex selection</font>"
                    onCheckedChanged: {
                        model.toggleVertexSelection(checked);
                    }
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: true
                    text: "<font color=\"white\">Allow Part selection</font>"
                    onCheckedChanged: {
                        model.togglePartSelection(checked);
                    }
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: true
                    text: "<font color=\"white\">Allow Geometry selection</font>"
                    onCheckedChanged: {
                        model.toggleGeometrySelection(checked);
                    }
                }

                CheckBox {
                    Layout.columnSpan: 2
                    checked: true
                    text: "<font color=\"white\">Allow Mid Vertex selection</font>"
                    onCheckedChanged: {
                        model.toggleMidVertexSelection(checked);
                    }
                }
            }
        }
    }
}
