// [WriteFile Name=AddGraphicsWithRenderer, Category=DisplayInformation]
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
import Esri.ArcGISRuntime 100.13

Rectangle {
    width: 800
    height: 600

    property var rtCreate: ArcGISRuntimeEnvironment.createObject

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        // Set and keep the focus on MapView to enable keyboard navigation
        onFocusChanged: focus = true;
        Component.onCompleted: {
            forceActiveFocus();
        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
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

        // graphics overlay to display the heart graphic
        GraphicsOverlay {
            id: heartGraphicsOverlay

            renderer: SimpleRenderer {
                symbol: SimpleFillSymbol {
                    style: Enums.SimpleFillSymbolStyleSolid
                    color: "red"
                    outline: SimpleLineSymbol {
                        style: Enums.SimpleLineSymbolStyleSolid
                        color: "black"
                        width: 1
                    }
                }
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

    // The heart graphic to append the heart geometry to
    Graphic {
        id: heartGraphic
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

        // Create the heart graphic with curved line segments
        heartGraphic.geometry = createHeartGeometry(40e5, 5e5, 10e5, Factory.SpatialReference.createWebMercator());
        heartGraphicsOverlay.graphics.append(heartGraphic);
    }

    function createHeartGeometry(centerX, centerY, sideLength, sr) {
        const minX = centerX - 0.5 * sideLength;
        const minY = centerY - 0.5 * sideLength;
        const arcRadius = sideLength * 0.25;

        // Variables used for arcs
        const rotationAngle = 0;
        const isMinor = false;
        const isCounterClockwise = false;
        const semiMajorAxis = sideLength * 0.25;
        const minorMajorRatio = 1;

        // Bottom left curve
        const leftCurveStart = rtCreate("Point", {x: centerX, y: minY, spatialReference: sr});
        const leftControlPoint1 = rtCreate("Point", {x: centerX, y: minY + 0.25 * sideLength, spatialReference: sr});
        const leftControlPoint2 = rtCreate("Point", {x: minX, y: centerY, spatialReference: sr});
        const leftCurveEnd = rtCreate("Point", {x: minX, y: minY + 0.75 * sideLength, spatialReference: sr});

        const leftCurveSegment = Factory.CubicBezierSegment.createWithPoints(
                                   leftCurveStart, leftControlPoint1, leftControlPoint2, leftCurveEnd, sr);

        // Top left arc
        const leftArcStart = leftCurveEnd;
        const leftArcEnd = rtCreate("Point", {x: centerX, y: minY + 0.75 * sideLength, spatialReference: sr});

        const leftArc = Factory.EllipticArcSegment.createEllipticArcWithStartAndEndpoints(
                          leftArcStart, leftArcEnd, rotationAngle, isMinor, isCounterClockwise, semiMajorAxis, minorMajorRatio, sr);

        // Top right arc
        const rightArcStart = leftArcEnd;
        const rightArcEnd = rtCreate("Point", {x: minX + sideLength, y: minY + 0.75 * sideLength, spatialReference: sr});

        const rightArc = Factory.EllipticArcSegment.createEllipticArcWithStartAndEndpoints(
                           rightArcStart, rightArcEnd, rotationAngle, isMinor, isCounterClockwise, semiMajorAxis, minorMajorRatio, sr);

        // Bottom right curve
        const rightCurveStart = rightArcEnd;
        const rightControlPoint1 = rtCreate("Point", {x: minX + sideLength, y: centerY, spatialReference: sr});
        const rightControlPoint2 = leftControlPoint1;
        const rightCurveEnd = leftCurveStart;

        const rightCurveSegment = Factory.CubicBezierSegment.createWithPoints(
                                    rightCurveStart, rightControlPoint1, rightControlPoint2, rightCurveEnd, sr);

        // Create a part with each of the segments
        const part = rtCreate("Part", {spatialReference: sr});
        part.addSegment(leftCurveSegment);
        part.addSegment(leftArc);
        part.addSegment(rightArc);
        part.addSegment(rightCurveSegment);

        const polygonBuilder = rtCreate("PolygonBuilder", {spatialReference: sr});
        polygonBuilder.parts.addPart(part);

        return polygonBuilder.geometry;
    }
}
