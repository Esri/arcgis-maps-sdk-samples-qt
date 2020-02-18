// [WriteFile Name=NearestVertex, Category=Geometry]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Controls 2.4
import Esri.ArcGISRuntime 100.8

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        GraphicsOverlay {
            id: graphicsOverlay
        }

        SimpleFillSymbol {
            id: fillSymbol
            style: Enums.SimpleFillSymbolStyleForwardDiagonal
            color: "lime"
            SimpleLineSymbol {
                style: Enums.SimpleLineSymbolStyleSolid
                color: "lime"
                width: 2.0
            }
        }

        Graphic {
            id: polygonGraphic
            symbol: fillSymbol
        }

        PolygonBuilder {
            id: polygonBuilder
            spatialReference: SpatialReference { wkid: 102100 }
        }

        Component.onCompleted: {
            polygonBuilder.addPointXY(-5991501.677830, 5599295.131468);
            polygonBuilder.addPointXY(-6928550.398185, 2087936.739807);
            polygonBuilder.addPointXY(-3149463.800709, 1840803.011362);
            polygonBuilder.addPointXY(-1563689.043184, 3714900.452072);
            polygonBuilder.addPointXY(-3180355.516764, 5619889.608838);
            polygonGraphic.geometry = polygonBuilder.geometry;
            graphicsOverlay.graphics.append(polygonGraphic);
        }

        Map {
            BasemapTopographic {}

            ViewpointCenter {
                Point {
                    x: -4487263.495911
                    y: 3699176.480377
                    SpatialReference {wkid: 102100}
                }
                targetScale: 50000000
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var clickedPoint = mapView.screenToLocation(mouseX, mouseY);
            var clickedPointSymbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {style: "SimpleMarkerSymbolStyleX", color: "orange", size: 15});
            var clickedPointGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: clickedPointSymbol});
            graphicsOverlay.graphics.append(clickedPointGraphic);

            var nearestVertexPoint = GeometryEngine.nearestVertex(polygonBuilder.geometry, clickedPoint);
            var nearestVertexSymbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {style: "SimpleMarkerSymbolStyleCircle", color: "blue", size: 15});
            var nearestVertexGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: nearestVertexPoint.coordinate, symbol: nearestVertexSymbol});
            graphicsOverlay.graphics.append(nearestVertexGraphic);

            var nearestCoordinateResult = GeometryEngine.nearestCoordinate(polygonBuilder.geometry, clickedPoint);
            var nearestCoordinateSymbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {style: "SimpleMarkerSymbolStyleDiamond", color: "red", size: 10});
            var nearestCoordinateGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: nearestCoordinateResult.coordinate, symbol: nearestCoordinateSymbol});
            graphicsOverlay.graphics.append(nearestCoordinateGraphic);

            distancesLabel.text = "Vertex distance: " + Math.round(nearestVertexPoint.distance/1000.0) + " km\nCoordinate distance: " + Math.round(nearestCoordinateResult.distance/1000.0) + " km";
            distancesLabel.font.pointSize = 14;
            distancesLabel.leftPadding = 5;
            labelRectangle.visible = "true";
        }
    }

    Rectangle
    {
        id: labelRectangle
        width: 350
        height: 60
        visible: false
        color: "white"
        border.color: "black"
        border.width: 2
        Label {
            id: distancesLabel
        }
    }
}


