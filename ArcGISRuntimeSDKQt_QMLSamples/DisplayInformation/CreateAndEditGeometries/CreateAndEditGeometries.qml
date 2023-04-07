// [WriteFile Name=CreateAndEditGeometries, Category=DisplayInformation]
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
                    spatialReference: Factory.SpatialReference.createWgs84()
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

                            const tolerance = 22;
                            const returnPopupsOnly = false;
                            mapView.identifyGraphicsOverlay(geometriesOverlay, mouse.x, mouse.y, tolerance, returnPopupsOnly);
                        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus !== Enums.TaskStatusCompleted)
                return;

            if (mapView.identifyGraphicsOverlayResult.graphics.length === 0)
                return; // No graphic at location of tap.
            else if (mapView.identifyGraphicsOverlayResult.graphics.length > 1) {
                console.log(qsTr("Multiple graphics identifed - cannot identify the intended graphic."));
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
                    text: "Create"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                GeometryEditorButton {
                    id: pointButton
                    buttonName: qsTr("Point")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/point-32.png"
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
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/multipoint-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: {
                        geometryEditor.tool = vertexTool;
                        toolCombo.currentIndex = 0;
                        geometryEditor.startWithGeometryType(Enums.GeometryTypeMultipoint);
                    }
                }

                Rectangle { // Used to create a space between the point/multipoint and polyline/polygon buttons.
                    id: spacer
                    height: 10
                    opacity: 0
                    Layout.columnSpan: 2
                }

                GeometryEditorButton {
                    id: lineButton
                    buttonName: qsTr("Line")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/line-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: geometryEditor.startWithGeometryType(Enums.GeometryTypePolyline);
                }

                GeometryEditorButton {
                    id: polygonButton
                    buttonName: qsTr("Polygon")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/polygon-32.png"
                    checkable: true
                    enabled: !geometryEditor.started
                    onClicked: geometryEditor.startWithGeometryType(Enums.GeometryTypePolygon);
                }

                ComboBox {
                    id: toolCombo
                    model: [qsTr("VertexTool"), qsTr("FreehandTool")]
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
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
                        default:
                            geometryEditor.tool = vertexTool;
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
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/undo-32.png"
                    enabled: geometryEditor.started && geometryEditor.canUndo
                    onClicked: geometryEditor.undo();
                }

                GeometryEditorButton {
                    id: redoButton
                    buttonName: qsTr("Redo")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/redo-32.png"
                    enabled: geometryEditor.started && geometryEditor.canRedo
                    onClicked: geometryEditor.redo();
                }

                GeometryEditorButton {
                    id: deleteVertexButton
                    buttonName: qsTr("Delete selected element")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/erase-32.png"
                    Layout.columnSpan: 2
                    enabled: geometryEditor.started && geometryEditor.selectedElement
                    onClicked: geometryEditor.deleteSelectedElement();
                }

                GeometryEditorButton {
                    id: saveEditsButton
                    buttonName: qsTr("Stop and save edits")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/check-circle-32.png"
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
                    buttonName: qsTr("Stop (discards edits)")
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/circle-disallowed-32.png"
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
                    iconPath: "qrc:/Samples/DisplayInformation/CreateAndEditGeometries/iconAssets/trash-32.png"
                    Layout.columnSpan: 2
                    enabled: !geometryEditor.started && geometriesOverlay.graphics.count > 0;
                    onClicked: geometriesOverlay.graphics.clear();
                }
            }
        }
    }

    function createInitialGraphics() {
        let houseCoord = [-9.59309629, 53.0830063];
        let outbuildingCoords = [
                [-9.59386587, 53.08289651],
                [-9.59370896, 53.08234917],
                [-9.59330546, 53.082564],
                [-9.59341755, 53.08286662],
                [-9.59326997, 53.08304595],
                [-9.59246485, 53.08294507],
                [-9.59250034, 53.08286101],
                [-9.59241815, 53.08284607],
                [-9.59286835, 53.08311506],
                [-9.59307943, 53.08234731]
            ];
        let road1Coords = [
                [-9.59486423, 53.08169453],
                [-9.5947812, 53.08175431],
                [-9.59475464, 53.08189379],
                [-9.59494393, 53.08213622],
                [-9.59464173, 53.08240521],
                [-9.59413694, 53.08260115],
                [-9.59357903, 53.0829266],
                [-9.59335984, 53.08311589],
                [-9.59318051, 53.08316903],
                [-9.59301779, 53.08322216],
                [-9.59264252, 53.08370038],
                [-9.59250636, 53.08383986]
            ];
        let road2Coords = [
                [-9.59400079, 53.08136244],
                [-9.59395761, 53.08149528],
                [-9.59368862, 53.0817045],
                [-9.59358235, 53.08219267],
                [-9.59331667, 53.08290335],
                [-9.59314398, 53.08314246],
                [-9.5930676, 53.08330519],
                [-9.59303439, 53.08351109],
                [-9.59301447, 53.08363728],
                [-9.59293809, 53.08387307]
            ];
        let boundaryCoords = [
                [-9.59350122, 53.08320723],
                [-9.59345177, 53.08333534],
                [-9.59309789, 53.08327198],
                [-9.59300344, 53.08317992],
                [-9.59221827, 53.08304034],
                [-9.59220706, 53.08287782],
                [-9.59229486, 53.08280871],
                [-9.59236398, 53.08268915],
                [-9.59255263, 53.08256769],
                [-9.59265165, 53.08237906],
                [-9.59287552, 53.08241478],
                [-9.59292812, 53.0823012],
                [-9.5932294, 53.08235022],
                [-9.59342188, 53.08260009],
                [-9.59354382, 53.08238728],
                [-9.59365852, 53.08203535],
                [-9.59408443, 53.08210446],
                [-9.59448232, 53.08224456],
                [-9.5943609, 53.08243697],
                [-9.59458319, 53.08245939],
                [-9.59439639, 53.08264619],
                [-9.59433288, 53.0827975],
                [-9.59404707, 53.08323649],
                [-9.59350122, 53.08320723]
            ];

        createPolygonAndAddToGraphics(boundaryCoords);
        createLineAndAddToGraphics(road1Coords);
        createLineAndAddToGraphics(road2Coords);
        createMultipointAndAddToGraphics(outbuildingCoords);
        createPointAndAddToGraphics(houseCoord);
    }

    function createPointAndAddToGraphics(pointCoord) {
        let pointBuilder = ArcGISRuntimeEnvironment.createObject("PointBuilder", {x: pointCoord[0], y: pointCoord[1], spatialReference: Factory.SpatialReference.createWgs84()});
        const pointGeometry = pointBuilder.geometry;
        const pointGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: pointGeometry, symbol: pointSymbol});
        geometriesOverlay.graphics.append(pointGraphic);
    }

    function createMultipointAndAddToGraphics(multipointCoords) {
        let multipointBuilder = ArcGISRuntimeEnvironment.createObject("MultipointBuilder", {spatialReference: Factory.SpatialReference.createWgs84()});
        multipointCoords.forEach( (coordinate) => {multipointBuilder.points.addPointXY(coordinate[0], coordinate[1])});
        const multipointGeometry = multipointBuilder.geometry;
        const multipointGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: multipointGeometry, symbol: multiPointSymbol});
        geometriesOverlay.graphics.append(multipointGraphic);
    }

    function createLineAndAddToGraphics(lineCoords) {
        let polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {spatialReference: Factory.SpatialReference.createWgs84()});
        lineCoords.forEach( (coordinate) => {polylineBuilder.addPointXY(coordinate[0], coordinate[1])});
        const polylineGeometry = polylineBuilder.geometry;
        const polylineGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: polylineGeometry, symbol: polylineSymbol});
        geometriesOverlay.graphics.append(polylineGraphic);
    }

    function createPolygonAndAddToGraphics(polygonCoords) {
        let polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {spatialReference: Factory.SpatialReference.createWgs84()});
        polygonCoords.forEach( (coordinate) => {polygonBuilder.addPointXY(coordinate[0], coordinate[1])});
        const polygonGeometry = polygonBuilder.geometry;
        const polygonGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: polygonGeometry, symbol: polygonSymbol});
        geometriesOverlay.graphics.append(polygonGraphic);
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
