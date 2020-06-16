// [WriteFile Name=CreateGeometries, Category=Geometry]
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

        Map {
            BasemapTopographic {}

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                addGraphics();
            }
        }

        // Create a Graphics Overlay
        GraphicsOverlay {
            id: graphicsOverlay
        }
    }

    // function to add Graphics to a Graphics Overlay
    function addGraphics() {
        // Create a simple fill symbol - used to render a polygon covering Colorado.
        const simpleFillSymbol = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol", {
                                                                           style: Enums.SimpleFillSymbolStyleCross,
                                                                           color: "blue"
                                                                       });

        // Create a simple line symbol - used to render a polyline border between California and Nevada.
        const simpleLineSymbol = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol", {
                                                                           style: Enums.SimpleLineSymbolStyleSolid,
                                                                           color: "blue",
                                                                           width: 3
                                                                       });

        // Create a simple marker symbol - used to render multi-points for various state capital locations in the Western United States.
        const simpleMarkerSymbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                             style: Enums.SimpleMarkerSymbolStyleTriangle,
                                                                             color: "blue",
                                                                             size: 14
                                                                         });

        // Create a simple marker symbol - used to render a map point where the Esri headquarters is located.
        const simpleMarkerSymbol2 = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                              style: Enums.SimpleMarkerSymbolStyleDiamond,
                                                                              color: "red",
                                                                              size: 18
                                                                          });

        // Add a graphic to the graphic collection - polygon with a simple fill symbol.
        graphicsOverlay.graphics.append(createGraphic(createPolygon(), simpleFillSymbol));

        // Add a graphic to the graphic collection - polyline with a simple line symbol.
        graphicsOverlay.graphics.append(createGraphic(createPolyline(), simpleLineSymbol));

        // Add a graphic to the graphic collection - map point with a simple marker symbol.
        graphicsOverlay.graphics.append(createGraphic(createMapPoint(), simpleMarkerSymbol2));

        // Add a graphic to the graphic collection - multi-point with a simple marker symbol.
        graphicsOverlay.graphics.append(createGraphic(createMultipoint(), simpleMarkerSymbol));

        // Zoom to the extent of an envelope with some padding (10 device-independent pixels).
        mapView.setViewpointGeometryAndPadding(createEnvelope(), 10);
    }

    function createEnvelope() {
        // Return an envelope that covers the extent of the western United States.
        return ArcGISRuntimeEnvironment.createObject("Envelope", {
                                                         xMin: -123.0,
                                                         yMin: 33.5,
                                                         xMax: -101.0,
                                                         yMax: 48.0,
                                                         spatialReference: Factory.SpatialReference.createWgs84()
                                                     });
    }


    function createPolygon() {
        // Create a polygon builder
        const polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {
                                                                         spatialReference: Factory.SpatialReference.createWgs84()
                                                                     });

        // add points to the builder that approximates the boundary of Colorado.
        polygonBuilder.addPointXY(-109.048, 40.998);
        polygonBuilder.addPointXY(-102.047, 40.998);
        polygonBuilder.addPointXY(-102.037, 36.989);
        polygonBuilder.addPointXY(-109.048, 36.998);

        // Return the geometry.
        return polygonBuilder.geometry;
    }

    function createPolyline() {
        // Create a polyline builder
        const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {
                                                                          spatialReference: Factory.SpatialReference.createWgs84()
                                                                      });

        // add points to the builder that approximates the border between California and Nevada.
        polylineBuilder.addPointXY(-119.992, 41.989);
        polylineBuilder.addPointXY(-119.994, 38.994);
        polylineBuilder.addPointXY(-114.620, 35.0);

        // Return the geometry.
        return polylineBuilder.geometry;
    }

    function createMapPoint() {
        // Return a map point where the Esri headquarters is located.
        return ArcGISRuntimeEnvironment.createObject("Point", {
                                                         x: -117.195800,
                                                         y: 34.056295,
                                                         spatialReference: Factory.SpatialReference.createWgs84()
                                                     });
    }

    function createMultipoint() {
        // Create a polygon builder
        const multipointBuilder = ArcGISRuntimeEnvironment.createObject("MultipointBuilder", {
                                                                            spatialReference: Factory.SpatialReference.createWgs84()
                                                                        });

        // add points to the builder representing various state capital locations in the Western United States.
        multipointBuilder.points.addPointXY(-121.491014, 38.579065); // - Sacramento, CA
        multipointBuilder.points.addPointXY(-122.891366, 47.039231); // - Olympia, WA
        multipointBuilder.points.addPointXY(-123.043814, 44.93326); // - Salem, OR
        multipointBuilder.points.addPointXY(-119.766999, 39.164885); // - Carson City, NV

        // Return the geometry.
        return multipointBuilder.geometry;
    }

    // helper to create a graphic
    function createGraphic(geometry, symbol) {
        return ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                         geometry: geometry,
                                                         symbol: symbol
                                                     });
    }
}
