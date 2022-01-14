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

import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import Esri.ArcGISRuntime 100.14

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    enum DrawingModes {
        NotDrawing,
        Point,
        Multipoint,
        Line,
        Polygon
    }

    property int drawStatus: SketchOnMap.DrawingModes.NotDrawing

    MapView {
        id: mapView
        anchors.fill: parent

        // Force focus to remain on MapView so SketchEditor will respond to keystrokes
        onFocusChanged: focus = true;

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISImagery
            }

            initialViewpoint: ViewpointCenter {
                Point {
                    y: 64.3286
                    x: -15.5314
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 100000
            }
        }

        SketchEditor {
            id: sketchEditor
        }

        GraphicsOverlay {
            id: sketchOverlay
        }
    }

    // Display an option to delete the selected vertex if the user right-clicks or taps and holds when the sketch editor is started
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        enabled: sketchEditor.started
        onClicked: {
            contextMenu.popup()
        }

        Menu {
            id: contextMenu
            width: actionComponent.width
            Action {
                id: actionComponent
                text: "Delete"
                onTriggered: {
                    if (!sketchEditor.geometry.extent.empty)
                        sketchEditor.removeSelectedVertex();
                }
            }
        }
    }

    SimpleMarkerSymbol {
        id: pointSymbol
        style: Enums.SimpleMarkerSymbolStyleSquare
        color: "red"
        size: 10
    }

    SimpleMarkerSymbol {
        id: multiPointSymbol
        style: Enums.SimpleMarkerSymbolStyleSquare
        color: "blue"
        size: 10
    }

    SimpleLineSymbol {
        id: polylineSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "#90EE90"
        width: 3
    }

    SimpleFillSymbol {
        id: polygonSymbol
        style: Enums.SimpleFillSymbolStyleSolid
        color: "#7743A6C6"
        outline: SimpleLineSymbol {
            style: "SimpleLineSymbolStyleSolid"
            width: 3
            color: "#43A6C6"
        }
    }

    Control {
        id: control
        anchors.right: parent.right
        padding: 5
        width: 110

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

                Text {
                    id: geometryHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "Geometry"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                SketchEditorButton {
                    id: ptButton
                    buttonName: "Point"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/point-32.png"
                    highlighted: drawStatus === SketchOnMap.DrawingModes.Point

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePoint);
                        drawStatus = SketchOnMap.DrawingModes.Point;
                    }
                }

                SketchEditorButton {
                    id: mPtButton
                    buttonName: "Multipoint"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/point-32.png"
                    images: 2
                    highlighted: drawStatus === SketchOnMap.DrawingModes.Multipoint

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModeMultipoint);
                        drawStatus = SketchOnMap.DrawingModes.Multipoint;
                    }
                }

                SketchEditorButton {
                    id: lineButton
                    buttonName: "Line"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/line-32.png"
                    highlighted: drawStatus === SketchOnMap.DrawingModes.Line

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePolyline);
                        drawStatus = SketchOnMap.DrawingModes.Line;
                    }
                }

                SketchEditorButton {
                    id: polygonButton
                    buttonName: "Polygon"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/polygon-32.png"
                    highlighted: drawStatus === SketchOnMap.DrawingModes.Polygon

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePolygon);
                        drawStatus = SketchOnMap.DrawingModes.Polygon;
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

                    enabled: sketchEditor.started

                    onClicked: sketchEditor.undo();
                }

                SketchEditorButton {
                    id: redoButton
                    buttonName: "Redo"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/redo-32.png"

                    enabled: sketchEditor.started

                    onClicked: sketchEditor.redo();
                }

                SketchEditorButton {
                    id: deleteVertexButton
                    buttonName: "Delete selected vertex"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/erase-32.png"
                    columnSpan: 2

                    enabled: sketchEditor.started

                    onClicked: {
                        if (!sketchEditor.geometry.extent.empty) {
                            sketchEditor.removeSelectedVertex();
                        }
                    }
                }

                SketchEditorButton {
                    id: saveEditsButton
                    buttonName: "Save edits"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/check-circle-32.png"
                    columnSpan: 2

                    enabled: sketchEditor.started

                    onClicked: {
                        drawStatus = SketchOnMap.DrawingModes.NotDrawing;
                        if (!sketchEditor.isSketchValid()) {
                            console.log("Unable to save sketch. Sketch is not valid.");
                            return;
                        }

                        // To save the sketch, create a graphic with the sketch's geometry before stopping the sketchEditor
                        const graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
                        graphic.geometry = sketchEditor.geometry;

                        switch (sketchEditor.creationMode) {
                        case Enums.SketchCreationModePoint:
                            graphic.symbol = pointSymbol
                            break;
                        case Enums.SketchCreationModeMultipoint:
                            graphic.symbol = multiPointSymbol
                            break;
                        case Enums.SketchCreationModePolyline:
                            graphic.symbol = polylineSymbol
                            break;
                        case Enums.SketchCreationModePolygon:
                            graphic.symbol = polygonSymbol
                            break;

                        default:
                            break;
                        }

                        sketchOverlay.graphics.append(graphic);
                        sketchEditor.stop();
                    }
                }

                SketchEditorButton {
                    id: discardEditsButton
                    buttonName: "Discard edits"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/circle-disallowed-32.png"
                    columnSpan: 2

                    enabled: sketchEditor.started

                    onClicked: {
                        drawStatus = SketchOnMap.DrawingModes.NotDrawing;
                        sketchEditor.stop();
                    }
                }

                SketchEditorButton {
                    id: clearGraphicsButton
                    buttonName: "Clear graphics"
                    iconPath: "qrc:/Samples/DisplayInformation/SketchOnMap/iconAssets/trash-32.png"
                    columnSpan: 2
                    enabled: sketchOverlay.graphics.count > 0;

                    onClicked: {
                        sketchOverlay.graphics.clear();
                    }
                }
            }
        }
    }
}
