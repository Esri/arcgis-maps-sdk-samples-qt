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
import Esri.ArcGISRuntime 100.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var drawStatus: ""

    onFocusChanged: {
        mapView.focus = true
    }
    MapView {
        id: mapView
        anchors.fill: parent

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
        acceptedButtons: {
            Qt.RightButton
            Qt.TapAndHoldGesture
        }
        enabled: sketchEditor.started
        onClicked: {
            contextMenu.popup()
        }

        Menu {
            id: contextMenu
            width: 75
            Action {
                text: "Delete"
                onTriggered: sketchEditor.removeSelectedVertex();
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
                    text: qsTr("Geometry")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                SketchEditorButton {
                    id: ptButton
                    buttonName: "Point"
                    iconPath: "iconAssets/point-32.png"
                    highlighted: drawStatus === "point"

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePoint);
                        drawStatus = "point";
                    }
                }

                SketchEditorButton {
                    id: mPtButton
                    buttonName: "Multipoint"
                    iconPath: "iconAssets/point-32.png"
                    images: 2
                    highlighted: drawStatus === "multiPoint"

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModeMultipoint);
                        drawStatus = "multiPoint";
                    }
                }

                SketchEditorButton {
                    id: lineButton
                    buttonName: "Line"
                    iconPath: "iconAssets/line-32.png"
                    highlighted: drawStatus === "line"

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePolyline);
                        drawStatus = "line";
                    }
                }

                SketchEditorButton {
                    id: polygonButton
                    buttonName: "Polygon"
                    iconPath: "iconAssets/polygon-32.png"
                    highlighted: drawStatus === "polygon"

                    enabled: !sketchEditor.started

                    onClicked: {
                        sketchEditor.startWithCreationMode(Enums.SketchCreationModePolygon);
                        drawStatus = "polygon";
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
                    text: qsTr("Editing")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                SketchEditorButton {
                    id: undoButton
                    buttonName: "Undo"
                    iconPath: "iconAssets/undo-32.png"

                    enabled: sketchEditor.started

                    onClicked: sketchEditor.undo();
                }

                SketchEditorButton {
                    id: redoButton
                    buttonName: "Redo"
                    iconPath: "iconAssets/redo-32.png"

                    enabled: sketchEditor.started

                    onClicked: sketchEditor.redo();
                }

                SketchEditorButton {
                    id: saveEditsButton
                    buttonName: "Save edits"
                    iconPath: "iconAssets/check-circle-32.png"
                    columnSpan: 2

                    enabled: sketchEditor.started

                    onClicked: {
                        drawStatus = "";
                        if (!sketchEditor.isSketchValid()) {
                            console.log("Unable to save sketch. Sketch is not valid.");
                            return;
                        }

                        // To save the sketch, create a graphic with the sketch's geometry before stopping the sketchEditor
                        var graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
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
                    iconPath: "iconAssets/circle-disallowed-32.png"
                    columnSpan: 2

                    enabled: sketchEditor.started

                    onClicked: {
                        drawStatus = "";
                        sketchEditor.stop();
                    }
                }

                SketchEditorButton {
                    id: clearGraphicsButton
                    buttonName: "Clear graphics"
                    iconPath: "iconAssets/trash-32.png"
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
