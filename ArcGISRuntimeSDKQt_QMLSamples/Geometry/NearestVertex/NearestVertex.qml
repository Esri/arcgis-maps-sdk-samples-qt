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
import Esri.ArcGISRuntime 100.9

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

        Graphic {
            id: polygonGraphic
            SimpleFillSymbol {
                style: Enums.SimpleFillSymbolStyleForwardDiagonal
                color: "lime"
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "lime"
                    width: 2.0
                }
            }
        }

        PolygonBuilder {
            id: polygonBuilder
            SpatialReference { wkid: 3857 }
        }

        Graphic {
            id:  clickedPointGraphic
            SimpleMarkerSymbol {
                style: Enums.SimpleMarkerSymbolStyleX
                color: "yellow"
                size: 15
            }
        }

        Graphic {
            id: nearestVertexGraphic
            SimpleMarkerSymbol {
                style: Enums.SimpleMarkerSymbolStyleCircle
                color: "blue"
                size: 15
            }
        }

        Graphic {
            id: nearestCoordinateGraphic
            SimpleMarkerSymbol {
                style: Enums.SimpleMarkerSymbolStyleDiamond
                color: "red"
                size: 10
            }
        }

        Component.onCompleted: {
            // create polygon in middle of Atlantic Ocean
            polygonBuilder.addPointXY(-5991501.677830, 5599295.131468);
            polygonBuilder.addPointXY(-6928550.398185, 2087936.739807);
            polygonBuilder.addPointXY(-3149463.800709, 1840803.011362);
            polygonBuilder.addPointXY(-1563689.043184, 3714900.452072);
            polygonBuilder.addPointXY(-3180355.516764, 5619889.608838);
            polygonGraphic.geometry = polygonBuilder.geometry;

            // append graphics to overlay
            graphicsOverlay.graphics.append(polygonGraphic);
            graphicsOverlay.graphics.append(clickedPointGraphic);
            graphicsOverlay.graphics.append(nearestVertexGraphic);
            graphicsOverlay.graphics.append(nearestCoordinateGraphic);
        }

        Map {
            BasemapTopographic {}

            ViewpointCenter {
                Point {
                    x: -4487263.495911
                    y: 3699176.480377
                    SpatialReference {wkid: 3857}
                }
                targetScale: 50000000
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            const clickedPoint = mapView.screenToLocation(mouseX, mouseY);
            // normalizing the geometry before performing geometric operations
            const normalizedPoint = GeometryEngine.normalizeCentralMeridian(clickedPoint);
            clickedPointGraphic.geometry = normalizedPoint;

            const nearestVertexPoint = GeometryEngine.nearestVertex(polygonBuilder.geometry, normalizedPoint);
            nearestVertexGraphic.geometry = nearestVertexPoint.coordinate;

            const nearestCoordinateResult = GeometryEngine.nearestCoordinate(polygonBuilder.geometry, normalizedPoint);
            nearestCoordinateGraphic.geometry = nearestCoordinateResult.coordinate;

            distancesLabel.text = `Vertex distance: ${(nearestVertexPoint.distance/1000.0).toFixed()} km
Coordinate distance: ${(nearestCoordinateResult.distance/1000.0).toFixed()} km` ;
        }
    }

    // create box to display label with distances to nearest vertex and coordinate
    Rectangle
    {
        id: labelRectangle
        width: childrenRect.width
        height: childrenRect.height
        anchors {
            top: parent.top
            left: parent.left
        }
        visible: true
        color: "white"
        border.color: "black"
        border.width: 2
        Label {
            id: distancesLabel
            font.pointSize: 14
            padding: 5
            text: "Vertex Distance: 0 km\nCoordinate distance: 0 km"
        }
    }
}


