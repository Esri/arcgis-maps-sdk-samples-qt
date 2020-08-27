// [WriteFile Name=SpatialRelationships, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    MapView {
        id: mapView
        anchors.fill: parent

        SelectionProperties {
            color: "yellow"
        }

        Map {
            BasemapTopographic {}
        }

        // Add a GraphicsOverlay
        GraphicsOverlay {
            id: graphicsOverlay

            // Add Polygon Graphic
            Graphic {
                id: polygonGraphic

                SimpleFillSymbol {
                    SimpleLineSymbol {
                        color: "green"
                        width: 2
                    }
                    style: Enums.SimpleFillSymbolStyleForwardDiagonal
                    color: "green"
                }

                Component.onCompleted: {
                    const polyBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {spatialReference: Factory.SpatialReference.createWebMercator()});
                    polyBuilder.addPointXY(-5991501.677830, 5599295.131468);
                    polyBuilder.addPointXY(-6928550.398185, 2087936.739807);
                    polyBuilder.addPointXY(-3149463.800709, 1840803.011362);
                    polyBuilder.addPointXY(-1563689.043184, 3714900.452072);
                    polyBuilder.addPointXY(-3180355.516764, 5619889.608838);
                    polygonGraphic.geometry = polyBuilder.geometry;
                }
            }

            // Add Polyline Graphic
            Graphic {
                id: polylineGraphic

                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDash
                    color: "red"
                    width: 4
                }

                Component.onCompleted: {
                    const polyBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {spatialReference: Factory.SpatialReference.createWebMercator()});
                    polyBuilder.addPointXY(-4354240.726880, -609939.795721);
                    polyBuilder.addPointXY(-3427489.245210, 2139422.933233);
                    polyBuilder.addPointXY(-2109442.693501, 4301843.057130);
                    polyBuilder.addPointXY(-1810822.771630, 7205664.366363);
                    polylineGraphic.geometry = polyBuilder.geometry;
                }
            }

            // Add Point Graphic
            Graphic {
                id: pointGraphic

                SimpleMarkerSymbol {
                    color: "blue"
                    style: Enums.SimpleMarkerSymbolStyleCircle
                    size: 10
                }

                Point {
                    id: pointGeometry
                    x: -4487263.495911
                    y: 3699176.480377

                    SpatialReference {wkid: 3857}

                    Component.onCompleted: mapView.setViewpointCenterAndScale(pointGeometry, 200000000)
                }
            }
        }

        // connect to mouse click signal
        onMouseClicked: {
            mapView.identifyGraphicsOverlay(graphicsOverlay, mouse.x, mouse.y, 1 /*tolerance*/, false /*returnPopupsOnly*/);
        }       

        // connect to identify signal
        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus !== Enums.TaskStatusCompleted)
                return;

            const identifiedGraphics = identifyGraphicsOverlayResult.graphics;
            if (identifiedGraphics.length < 1)
                return;

            // get the first identified graphic
            const graphic = identifiedGraphics[0];

            // select the graphic
            graphicsOverlay.clearSelection();
            graphic.selected = true;

            // get the geometry
            const selectedGeometry = graphic.geometry;
            const selectedGeometryType = selectedGeometry.geometryType;

            // reset the output text
            pointText.text = "";
            lineText.text = "";
            polygonText.text = "";

            // populate the view with the spatial relationships the selected graphic has to the other graphics
            // ignore testing relationships between the geometry and itself
            if (selectedGeometryType !== Enums.GeometryTypePoint) {
                const pointRelationships = getSpatialRelationships(selectedGeometry, pointGraphic.geometry).toString();
                pointText.text = "Point: %1".arg(pointRelationships);
            }
            if (selectedGeometryType !== Enums.GeometryTypePolyline) {
                const polylineRelationships = getSpatialRelationships(selectedGeometry, polylineGraphic.geometry).toString();
                lineText.text = "Polyline: %1".arg(polylineRelationships);
            }
            if (selectedGeometryType !== Enums.GeometryTypePolygon) {
                const polygonRelationships = getSpatialRelationships(selectedGeometry, polygonGraphic.geometry).toString();
                polygonText.text = "Polygon: %1".arg(polygonRelationships);
            }
        }
    }

    // function to return list of relaionships
    function getSpatialRelationships(geom1, geom2) {
        const relationships = [];
        if (GeometryEngine.crosses(geom1, geom2))
            relationships.push("CROSSES");
        if (GeometryEngine.contains(geom1, geom2))
            relationships.push("CONTAINS");
        if (GeometryEngine.disjoint(geom1, geom2))
            relationships.push("DISJOINT");
        if (GeometryEngine.intersects(geom1, geom2))
            relationships.push("INTERSECTS");
        if (GeometryEngine.overlaps(geom1, geom2))
            relationships.push("OVERLAPS");
        if (GeometryEngine.touches(geom1, geom2))
            relationships.push("TOUCHES");
        if (GeometryEngine.within(geom1, geom2))
            relationships.push("WITHIN");
        return relationships;
    }

    Rectangle {
        anchors {
            fill: relationshipColumn
            margins: -10
        }
        opacity: 0.85
        radius: 5
        color: "#e2e2e2"
        border {
            color: "darkgray"
            width: 1
        }
    }

    Column {
        id: relationshipColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        spacing: 5

        Text {
            text: "Relationships:"
            font {
                pixelSize: 16
                bold: true
                family: "helvetica"
            }
        }

        Text {
            id: pointText
            visible: text.length > 0
            font.family: "helvetica"
        }

        Text {
            id: lineText
            visible: text.length > 0
            font.family: "helvetica"
        }

        Text {
            id: polygonText
            visible: text.length > 0
            font.family: "helvetica"
        }
    }
}
