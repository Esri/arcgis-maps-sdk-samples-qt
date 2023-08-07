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
        json: {"x":-1067898.5941336418,
            "y":6998366.62281668,
            "spatialReference":{"latestWkid":3857,"wkid":102100}
        }
    }

    Multipoint {
        id: outbuildings
        json: { "points":[[-1067984.2633873667,6998346.275524412],[-1067966.7962460662,6998244.838217341],
                          [-1067921.8788315312,6998284.652030533],[-1067934.356633254,6998340.736043775],
                          [-1067917.928102803,6998373.971131997],[-1067828.3025543755,6998355.2751036985],
                          [-1067832.2532831037,6998339.6963491095],[-1067823.1039341553,6998336.927537238],
                          [-1067873.2199689103,6998386.779271045],[-1067896.717287027,6998244.493509872]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}
        }
    }

    Polyline {
        id: road1
        json: {"paths":[[[-1068095.400314195,6998123.516933312],[-1068086.1574568744,6998134.595596774],
                         [-1068083.2008111987,6998160.444635851],[-1068104.272477611,6998205.373015125],
                         [-1068070.6317274934,6998255.223926861],[-1068014.4387617358,6998291.53695112],
                         [-1067952.3325046275,6998351.852074713],[-1067927.9323854405,6998386.933094863],
                         [-1067907.9694611565,6998396.781531972],[-1067889.8555536144,6998406.628127934],
                         [-1067848.0806883047,6998495.2573051425],[-1067832.9234264381,6998521.107512861]]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}}
    }

    Polyline {
        id: road2
        json: {"paths":[[[-1067999.2826130644,6998061.972995486],[-1067994.4758374519,6998086.5912549365],
                         [-1067964.5320076235,6998125.364611646],[-1067952.702085337,6998215.834656898],
                         [-1067923.126723023,6998347.543174588],[-1067903.902960158,6998391.857311896],
                         [-1067895.400377451,6998422.016122588],[-1067891.7034571616,6998460.175808636],
                         [-1067889.485972905,6998483.562838054],[-1067880.9833901986,6998527.262424822]]],
            "spatialReference":{"latestWkid":3857,"wkid":102100}}
    }

    Polygon {
        id: boundary
        json: {"rings":[[[-1067943.6707350488,6998403.861146136],[-1067938.165986229,6998427.603845685],
                         [-1067898.772244827,6998415.861291827],[-1067888.258118922,6998398.79977736],
                         [-1067800.8533943356,6998372.931433],[-1067799.6055028439,6998342.811726935],
                         [-1067809.3793541356,6998330.003658479],[-1067817.073757339,6998307.845804203],
                         [-1067838.074179277,6998285.335889136],[-1067849.0970352555,6998250.377631391],
                         [-1067874.0181296594,6998256.997504947],[-1067879.8735348752,6998235.948105213],
                         [-1067913.4118710614,6998245.032810278],[-1067934.8386466492,6998291.340503773],
                         [-1067948.4129453567,6998251.90101726],[-1067961.1812909506,6998186.679235192],
                         [-1068008.5933752744,6998199.487073545],[-1068052.886287466,6998225.451225095],
                         [-1068039.369874894,6998261.109909566],[-1068064.1150845021,6998265.264941532],
                         [-1068043.3206036221,6998299.884114488],[-1068036.250702762,6998327.926125365],
                         [-1068004.4344790983,6998409.283912439],[-1067943.6707350488,6998403.861146136]]],
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
