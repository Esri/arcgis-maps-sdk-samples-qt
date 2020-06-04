// [WriteFile Name=ConvexHull, Category=Geometry]
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
import Esri.ArcGISRuntime 100.9
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.6

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

        GraphicsOverlay {
            id: graphicsOverlay

            Graphic {
                id: inputsGraphic
                SimpleMarkerSymbol {
                    id: markerSymbol
                    style: Enums.SimpleMarkerSymbolStyleCircle
                    size: 10
                    color: "red"
                }
            }

            Graphic {
                id: convexHullGraphic
            }
        }

        SimpleFillSymbol {
            id: fillSymbol
            style: Enums.SimpleFillSymbolStyleNull
            color: "transparent"
            SimpleLineSymbol {
                id: lineSymbol
                style: Enums.SimpleLineSymbolStyleSolid
                width: 3
                color: "blue"
            }
        }

        MultipointBuilder {
            id: multipointBuilder
            spatialReference: mapView.spatialReference
        }

        onMouseClicked: {
            const clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
            multipointBuilder.points.addPoint(clickedPoint);
            inputsGraphic.geometry = multipointBuilder.geometry;
        }

        RowLayout {
            anchors {
                left: parent.left
                top: parent.top
            }

            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: "Convex hull"

                // display the convex hull
                onClicked: {
                    // normalizing the geometry before performing geometric operations
                    const normalizedPoints = GeometryEngine.normalizeCentralMeridian(multipointBuilder.geometry);
                    const convHull = GeometryEngine.convexHull(normalizedPoints);
                    if (convHull.geometryType === Enums.GeometryTypePoint) {
                        convexHullGraphic.symbol = markerSymbol;
                    } else if (convHull.geometryType === Enums.GeometryTypePolyline) {
                        convexHullGraphic.symbol = lineSymbol;
                    } else if (convHull.geometryType === Enums.GeometryTypePolygon) {
                        convexHullGraphic.symbol = fillSymbol;
                    }
                    else {
                        console.warn("Not a valid geometry");
                    }

                    convexHullGraphic.geometry = convHull;
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: "Reset"

                onClicked: {
                    multipointBuilder.points.removeAll();
                    inputsGraphic.geometry = null;
                    convexHullGraphic.geometry = null;
                }
            }
        }
    }
}
