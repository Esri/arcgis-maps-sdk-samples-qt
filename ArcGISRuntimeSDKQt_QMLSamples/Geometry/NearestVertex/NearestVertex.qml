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
import Esri.ArcGISRuntime 100.15

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
            spatialReference: statePlaneCaliforniaZone5SpatialReference
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
            polygonBuilder.addPointXY(6627416.41469281, 1804532.53233782);
            polygonBuilder.addPointXY(6669147.89779046, 2479145.16609522);
            polygonBuilder.addPointXY(7265673.02678292, 2484254.50442408);
            polygonBuilder.addPointXY(7676192.55880379, 2001458.66365744);
            polygonBuilder.addPointXY(7175695.94143837, 1840722.34474458);
            polygonGraphic.geometry = polygonBuilder.geometry;

            // append graphics to overlay
            graphicsOverlay.graphics.append(polygonGraphic);
            graphicsOverlay.graphics.append(clickedPointGraphic);
            graphicsOverlay.graphics.append(nearestVertexGraphic);
            graphicsOverlay.graphics.append(nearestCoordinateGraphic);

            mapView.map.basemap.baseLayers.append(usStatesGeneralizedLayer);
            mapView.setViewpointCenterAndScale(polygonGraphic.geometry.extent.center, 8e6);

            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            spatialReference: SpatialReference {
                wkid: 2229
            }
        }

        SpatialReference {
            id: statePlaneCaliforniaZone5SpatialReference
            wkid: 2229
        }

        FeatureLayer {
            id: usStatesGeneralizedLayer
            PortalItem {
                itemId: "99fd67933e754a1181cc755146be21ca"
            }
        }
    }

    MouseArea {
        onClicked: {
            const clickedPoint = mapView.screenToLocation(mouseX, mouseY);
            // normalizing the geometry before performing geometric operations
            const normalizedPoint = GeometryEngine.normalizeCentralMeridian(clickedPoint);
            clickedPointGraphic.geometry = normalizedPoint;

            const nearestVertexPoint = GeometryEngine.nearestVertex(polygonBuilder.geometry, normalizedPoint);
            nearestVertexGraphic.geometry = nearestVertexPoint.coordinate;

            const nearestCoordinateResult = GeometryEngine.nearestCoordinate(polygonBuilder.geometry, normalizedPoint);
            nearestCoordinateGraphic.geometry = nearestCoordinateResult.coordinate;

            distancesLabel.text = `Vertex distance: ${(nearestVertexPoint.distance/5280.0).toFixed()} mi
Coordinate distance: ${(nearestCoordinateResult.distance/5280.0).toFixed()} mi` ;
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
            text: "Vertex Distance: 0 mi\nCoordinate distance: 0 mi"
        }
    }
}


