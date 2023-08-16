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
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // When editing an existing geometry, a reference to the Graphic being used to visualise the
    // geometry is needed in order to update the graphic with any changes made during editing.
    property Graphic selectedGraphic: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISImagery
            }

            ViewpointCenter {
                center: Point {
                    x: -9.5920
                    y: 53.08230
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 5000
            }
        }

        geometryEditor: geometryEditor

        GraphicsOverlay {
            id: geometriesOverlay
        }

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();

            createInitialGraphics();
        }

        onMouseClicked: mouse => {
                            if (geometryEditor.started)
                                return;

                            const tolerance = 5;
                            const returnPopupsOnly = false;
                            mapView.identifyGraphicsOverlay(geometriesOverlay, mouse.x, mouse.y, tolerance, returnPopupsOnly);
                        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus !== Enums.TaskStatusCompleted)
                return;

            if (mapView.identifyGraphicsOverlayResult.graphics.length === 0)
                return; // No graphic at location of tap.
            else if (mapView.identifyGraphicsOverlayResult.graphics.length > 1) {
                console.log(qsTr("Multiple graphics identified - cannot identify the intended graphic."));
                return;
            }

            selectedGraphic = identifyGraphicsOverlayResult.graphics[0];

            // If a point or multipoint has been selected, we need to use a vertex tool - the UI also needs updating.
            if (selectedGraphic.geometry.geometryType === Enums.GeometryTypePoint || selectedGraphic.geometry.geometryType === Enums.GeometryTypeMultipoint) {
                geometryEditor.tool = vertexTool;
                toolCombo.currentIndex = 0;
            }

            geometryEditor.startWithGeometry(selectedGraphic.geometry);
            selectedGraphic.visible = false; // Reduce clutter in the view by hiding graphic during editing session.
        }
    }

    GeometryEditor {
        id: geometryEditor
        onStartedChanged: {
            // If an editing session has been started by selecting a graphic (i.e. the button for the geometry type was not
            // pressed) the checked property should be set on the appropriate button. This will highlight the button.
            if (started) {
                switch (geometry.geometryType) {
                case Enums.GeometryTypePoint:
                    pointButton.checked = true;
                    break;
                case Enums.GeometryTypeMultipoint:
                    multiPointButton.checked = true;
                    break;
                case Enums.GeometryTypePolyline:
                    lineButton.checked = true;
                    break;
                case Enums.GeometryTypePolygon:
                    polygonButton.checked = true;
                    break;
                }
            }
            // Once the editing session has stopped, return all buttons to unchecked state.
            else {
                pointButton.checked = false;
                multiPointButton.checked = false;
                lineButton.checked = false;
                polygonButton.checked = false;
            }
        }
    }

    VertexTool {
        id: vertexTool
    }

    FreehandTool {
        id: freehandTool
    }

    property ShapeTool arrowTool: Factory.ShapeTool.create(Enums.ShapeToolTypeArrow);

    property ShapeTool ellipseTool: Factory.ShapeTool.create(Enums.ShapeToolTypeEllipse);

    property ShapeTool rectangleTool: Factory.ShapeTool.create(Enums.ShapeToolTypeRectangle);

    property ShapeTool triangleTool: Factory.ShapeTool.create(Enums.ShapeToolTypeTriangle);

    SimpleMarkerSymbol {
        id: pointSymbol
        style: Enums.SimpleMarkerSymbolStyleSquare
        color: "orangered"
        size: 10
    }

    SimpleMarkerSymbol {
        id: multiPointSymbol
        style: Enums.SimpleMarkerSymbolStyleCircle
        color: "yellow"
        size: 5
    }

    SimpleLineSymbol {
        id: polylineSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "blue"
        width: 2
    }

    SimpleFillSymbol {
        id: polygonSymbol
        style: Enums.SimpleFillSymbolStyleSolid
        color: Qt.rgba(1, 0, 0, 0.3)
        outline: SimpleLineSymbol {
            style: Enums.SimpleLineSymbolStyleDash
            width: 1
            color: "black"
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
                    enabled: !geometryEditor.started
                    onClicked: {
                        geometryEditor.tool = vertexTool;
                        toolCombo.currentIndex = 0;
                        geometryEditor.startWithGeometryType(Enums.GeometryTypePoint);
                    }
                }

                GeometryEditorButton {
                    id: multiPointButton
                    buttonName: qsTr("Multipoint")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/multipoint-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: {
                        geometryEditor.tool = vertexTool;
                        toolCombo.currentIndex = 0;
                        geometryEditor.startWithGeometryType(Enums.GeometryTypeMultipoint);
                    }
                }

                GeometryEditorButton {
                    id: lineButton
                    buttonName: qsTr("Line")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/line-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: geometryEditor.startWithGeometryType(Enums.GeometryTypePolyline);
                }

                GeometryEditorButton {
                    id: polygonButton
                    buttonName: qsTr("Polygon")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/polygon-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: geometryEditor.startWithGeometryType(Enums.GeometryTypePolygon);
                }

                ComboBox {
                    id: toolCombo
                    model: [qsTr("VertexTool"), qsTr("FreehandTool"), qsTr("Arrow ShapeTool"), qsTr("Ellipse ShapeTool"), qsTr("Rectangle ShapeTool"), qsTr("Triangle ShapeTool")]
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

                    enabled: {
                        if (!geometryEditor.started)
                            false;
                        else {
                            switch (geometryEditor.geometry.geometryType) {
                            case Enums.GeometryTypePoint:
                            case Enums.GeometryTypeMultipoint:
                                false;
                                break;
                            case Enums.GeometryTypePolyline:
                            case Enums.GeometryTypePolygon:
                                true;
                            }
                        }
                    }
                    onActivated: {
                        switch (currentIndex) {
                        case 0: // Vertex Tool
                            geometryEditor.tool = vertexTool;
                            break;
                        case 1: // Freehand Tool
                            geometryEditor.tool = freehandTool;
                            break;
                        case 2: // ShapeTool with arrow shape type
                            geometryEditor.tool = arrowTool;
                            break;
                        case 3: // ShapeTool with ellipse shape type
                            geometryEditor.tool = ellipseTool;
                            break;
                        case 4: // ShapeTool with rectangle shape type
                            geometryEditor.tool = rectangleTool;
                            break;
                        case 5: // ShapeTool with triangle shape type
                            geometryEditor.tool = triangleTool;
                            break;
                        default:
                            geometryEditor.tool = vertexTool;
                        }
                    }
                }
            }

            CheckBox {
                id: uniformScaleCheckBox
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.columnSpan: 2
                text: "<font color=\"white\">Uniform scale</font>"
                enabled: {
                    if (!geometryEditor.started) {
                        false;
                    }
                    else {
                        switch (geometryEditor.geometry.geometryType) {
                        case Enums.GeometryTypePoint:
                            false;
                            break;
                        case Enums.GeometryTypeMultipoint:
                        case Enums.GeometryTypePolyline:
                        case Enums.GeometryTypePolygon:
                            true;
                        }
                    }
                }
                checked: geometryEditor.tool.configuration.scaleMode === Enums.GeometryEditorScaleModeUniform;
                onCheckStateChanged:
                {
                    vertexTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
                    freehandTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
                    arrowTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
                    ellipseTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
                    rectangleTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
                    triangleTool.configuration.scaleMode = (checked ? Enums.GeometryEditorScaleModeUniform : Enums.GeometryEditorScaleModeStretch)
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
                    enabled: geometryEditor.started && geometryEditor.canUndo
                    onClicked: geometryEditor.undo();
                }

                GeometryEditorButton {
                    id: redoButton
                    buttonName: qsTr("Redo")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/redo-32.png"
                    enabled: geometryEditor.started && geometryEditor.canRedo
                    onClicked: geometryEditor.redo();
                }

                GeometryEditorButton {
                    id: deleteVertexButton
                    buttonName: qsTr("Delete selected element")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/erase-32.png"
                    Layout.columnSpan: 2
                    enabled: geometryEditor.started && geometryEditor.selectedElement && geometryEditor.selectedElement.canDelete
                    onClicked: geometryEditor.deleteSelectedElement();
                }

                GeometryEditorButton {
                    id: saveEditsButton
                    buttonName: qsTr("Stop and save edits")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/check-circle-32.png"
                    Layout.columnSpan: 2
                    enabled: geometryEditor.started && geometryEditor.canUndo
                    onClicked: {
                        if (geometryEditor.geometry.empty) {
                            console.log(qsTr("Unable to save geometry. Geometry is empty."));
                            return;
                        }

                        if (selectedGraphic)
                            updateSelectedGraphic();
                        else
                            createNewGraphicAndAppendToGraphicsOverlay();
                    }
                }

                GeometryEditorButton {
                    id: discardEditsButton
                    buttonName: qsTr("Stop and discard edits")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/circle-disallowed-32.png"
                    Layout.columnSpan: 2
                    enabled: geometryEditor.started
                    onClicked: {
                        if (selectedGraphic) {
                            geometryEditor.stop();
                            selectedGraphic.visible = true;
                            selectedGraphic = null;
                        }
                        else
                            geometryEditor.stop();
                    }
                }

                GeometryEditorButton {
                    id: clearGraphicsButton
                    buttonName: qsTr("Delete all geometries")
                    iconPath: "qrc:/Samples/Geometry/CreateAndEditGeometries/iconAssets/trash-32.png"
                    Layout.columnSpan: 2
                    enabled: !geometryEditor.started && geometriesOverlay.graphics.count > 0;
                    onClicked: geometriesOverlay.graphics.clear();
                }
            }
        }
    }

    Point {
        id: house
        json: {"x":-1067898.59,
            "y":6998366.62,
            "spatialReference":{"latestWkid":3857,"wkid":102100}
        }
    }

    Multipoint {
        id: outbuildings
        json: { "points":[[-1067984.26,6998346.28],[-1067966.80,6998244.84],
                          [-1067921.88,6998284.65],[-1067934.36,6998340.74],
                          [-1067917.93,6998373.97],[-1067828.30,6998355.28],
                          [-1067832.25,6998339.70],[-1067823.10,6998336.93],
                          [-1067873.22,6998386.78],[-1067896.72,6998244.49]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}
        }
    }

    Polyline {
        id: road1
        json: {"paths":[[[-1068095.40,6998123.52],[-1068086.16,6998134.60],
                         [-1068083.20,6998160.44],[-1068104.27,6998205.37],
                         [-1068070.63,6998255.22],[-1068014.44,6998291.54],
                         [-1067952.33,6998351.85],[-1067927.93,6998386.93],
                         [-1067907.97,6998396.78],[-1067889.86,6998406.63],
                         [-1067848.08,6998495.26],[-1067832.92,6998521.11]]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}}
    }

    Polyline {
        id: road2
        json: {"paths":[[[-1067999.28,6998061.97],[-1067994.48,6998086.59],
                         [-1067964.53,6998125.37],[-1067952.70,6998215.84],
                         [-1067923.13,6998347.54],[-1067903.90,6998391.86],
                         [-1067895.40,6998422.02],[-1067891.70,6998460.18],
                         [-1067889.49,6998483.56],[-1067880.98,6998527.26]]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}}
    }

    Polygon {
        id: boundary
        json: {"rings":[[[-1067943.67,6998403.86],[-1067938.17,6998427.60],
                         [-1067898.77,6998415.86],[-1067888.26,6998398.80],
                         [-1067800.85,6998372.93],[-1067799.61,6998342.81],
                         [-1067809.38,6998330.00],[-1067817.07,6998307.85],
                         [-1067838.07,6998285.34],[-1067849.10,6998250.38],
                         [-1067874.02,6998256.00],[-1067879.87,6998235.95],
                         [-1067913.41,6998245.03],[-1067934.84,6998291.34],
                         [-1067948.41,6998251.90],[-1067961.18,6998186.68],
                         [-1068008.59,6998199.49],[-1068052.89,6998225.45],
                         [-1068039.37,6998261.11],[-1068064.12,6998265.26],
                         [-1068043.32,6998299.88],[-1068036.25,6998327.93],
                         [-1068004.43,6998409.28],[-1067943.67,6998403.86]]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}}
    }

    function createInitialGraphics() {

        const boundaryGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: boundary, symbol: polygonSymbol});
        geometriesOverlay.graphics.append(boundaryGraphic);

        const road1Graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: road1, symbol: polylineSymbol});
        geometriesOverlay.graphics.append(road1Graphic);

        const road2Graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: road2, symbol: polylineSymbol});
        geometriesOverlay.graphics.append(road2Graphic);

        const outbuildingsGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: outbuildings, symbol: multiPointSymbol});
        geometriesOverlay.graphics.append(outbuildingsGraphic);

        const houseGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: house, symbol: pointSymbol});
        geometriesOverlay.graphics.append(houseGraphic);
    }

    function updateSelectedGraphic() {
        selectedGraphic.geometry = geometryEditor.stop();
        selectedGraphic.visible = true;
        selectedGraphic = null;
    }

    function createNewGraphicAndAppendToGraphicsOverlay() {
        // To save the geometry, create a graphic using the output from the geometry editor.
        const graphic = ArcGISRuntimeEnvironment.createObject("Graphic");

        switch (geometryEditor.geometry.geometryType) {
        case Enums.GeometryTypePoint:
            graphic.symbol = pointSymbol
            break;
        case Enums.GeometryTypeMultipoint:
            graphic.symbol = multiPointSymbol
            break;
        case Enums.GeometryTypePolyline:
            graphic.symbol = polylineSymbol
            break;
        case Enums.GeometryTypePolygon:
            graphic.symbol = polygonSymbol
            break;
        default:
            break;
        }

        graphic.geometry = geometryEditor.stop();
        geometriesOverlay.graphics.append(graphic);
    }
}
