// [WriteFile Name=GORenderers, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent

        Map {
            BasemapTopographic {}
        }

        GraphicsOverlay {
            id: pointGraphicsOverlay
            // assign a render to the graphics overlay
            renderer: SimpleRenderer {
                symbol: pointSymbol
            }

            Graphic {
                geometry: Point {
                    x: 40e5
                    y: 40e5
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
            }
        }

        // graphics overlay for the line symbol
        GraphicsOverlay {
            id: lineGraphicsOverlay

            renderer: SimpleRenderer {
                symbol: lineSymbol
            }
        }

        // polygon graphics overlay
        GraphicsOverlay {
            id: polygonGraphicsOverlay

            // default property: renderer
            SimpleRenderer {
                symbol: fillSymbol
            }
        }
    }

    // the symbology for the point graphic
    SimpleMarkerSymbol {
        id: pointSymbol
        style: Enums.SimpleMarkerSymbolStyleDiamond
        color: "red"
        size: 10
    }

    // line symbol for the line graphic
    SimpleLineSymbol {
        id: lineSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "blue"
        width: 5
        antiAlias: true
    }

    // the line graphic
    Graphic {
        id: lineGraphic
    }

    // for the line graphic
    PolylineBuilder {
        id: polylineBuilder
        spatialReference: SpatialReference {
            wkid: 102100
        }
    }

    // symbology for the polygon graphic overlay's renderer
    SimpleFillSymbol {
        id: fillSymbol
        style: Enums.SimpleFillSymbolStyleSolid
        color: Qt.rgba(1, 1, 0, 0.7)
    }

    // the graphic for the polygon graphics overlay
    Graphic {
        id: polygonGraphic
    }

    // geometry for the polygon graphic
    PolygonBuilder {
        id: polygonBuilder
        spatialReference: SpatialReference {
            wkid: 102100
        }
    }

    Component.onCompleted: {
        // create the line by assigning points
        polylineBuilder.addPointXY(-10e5, 40e5);
        polylineBuilder.addPointXY(20e5, 50e5);
        // assign the graphics geometry to the line
        lineGraphic.geometry = polylineBuilder.geometry;
        // add the graphic to the polyline graphic overlay
        lineGraphicsOverlay.graphics.append(lineGraphic);

        // create the polygon by assigning points
        polygonBuilder.addPointXY(-20e5, 20e5);
        polygonBuilder.addPointXY(20e5, 20e5);
        polygonBuilder.addPointXY(20e5, -20e5);
        polygonBuilder.addPointXY(-20e5, -20e5);
        // assign the geometry of the graphic to be the polygon
        polygonGraphic.geometry = polygonBuilder.geometry;
        // add the graphic to the polygon graphics overlay
        polygonGraphicsOverlay.graphics.append(polygonGraphic);
    }
}
