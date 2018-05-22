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
import Esri.ArcGISRuntime 100.3

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
        }                

        // Add a GraphicsOverlay
        GraphicsOverlay {
            id: graphicsOverlay
            selectionColor: "yellow"

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
                    var polyBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {spatialReference: SpatialReference.createWebMercator()});
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
                    var polyBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {spatialReference: SpatialReference.createWebMercator()});
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

            var identifiedGraphics = identifyGraphicsOverlayResult.graphics;
            if (identifiedGraphics.length < 1)
                return;

            // get the first identified graphic
            var graphic = identifiedGraphics[0];

            // select the graphic
            graphicsOverlay.clearSelection();
            graphic.selected = true;
        }
    }
}
