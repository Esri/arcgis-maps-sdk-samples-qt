// [WriteFile Name=SpatialOperations, Category=Geometry]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var geometryOperations: ["None", "Union", "Difference", "Symmetric difference", "Intersection"]    
    property var geometry1
    property var geometry2

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapTopographic {}

            ViewpointCenter {
                Point {
                    x: -13453
                    y: 6710127
                    spatialReference: spatialRef
                }
                targetScale: 30000
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    addPolygons();
            }
        }

        GraphicsOverlay {
            id: inputOverlay
        }

        GraphicsOverlay {
            id: outputOverlay
        }
    }

    // Display a ComboBox with options for each operation
    ComboBox {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width
        model: geometryOperations

        onCurrentIndexChanged: applyGeometryOperation(currentIndex);

        Component.onCompleted : {
            for (var i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width);
            }
        }
        TextMetrics {
            id: metrics
            font: comboBox.font
        }
    }

    SpatialReference {
        id: spatialRef
        wkid: 3857
    }

    function addPolygons() {
        if (!inputOverlay)
            return;

        // create blue polygon
        var polygonBuilder1 = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", { spatialReference: spatialRef });
        polygonBuilder1.addPointXY(-13960, 6709400);
        polygonBuilder1.addPointXY(-14660, 6710000);
        polygonBuilder1.addPointXY(-13760, 6710730);
        polygonBuilder1.addPointXY(-13300, 6710500);
        polygonBuilder1.addPointXY(-13160, 6710100);

        var fillSymbol1 = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
        fillSymbol1.style = Enums.SimpleFillSymbolStyleSolid;
        fillSymbol1.color = "blue";
        geometry1 = polygonBuilder1.geometry;
        var graphic1 = ArcGISRuntimeEnvironment.createObject("Graphic");
        graphic1.geometry = geometry1;
        graphic1.symbol = fillSymbol1;
        inputOverlay.graphics.append(graphic1);

        // create green polygon
        // outer ring
        var outerRing = ArcGISRuntimeEnvironment.createObject("Part", { spatialReference: spatialRef });
        outerRing.addPointXY(-13060, 6711030);
        outerRing.addPointXY(-12160, 6710730);
        outerRing.addPointXY(-13160, 6709700);
        outerRing.addPointXY(-14560, 6710730);
        outerRing.addPointXY(-13060, 6711030);

        // inner ring
        var innerRing = ArcGISRuntimeEnvironment.createObject("Part", { spatialReference: spatialRef });
        innerRing.addPointXY(-13060, 6710910);
        innerRing.addPointXY(-14160, 6710630);
        innerRing.addPointXY(-13160, 6709900);
        innerRing.addPointXY(-12450, 6710660);
        innerRing.addPointXY(-13060, 6710910);

        var polygonBuilder2 = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", { spatialReference: spatialRef });
        polygonBuilder2.parts.addPart(outerRing);
        polygonBuilder2.parts.addPart(innerRing);
        geometry2 = polygonBuilder2.geometry;
        var fillSymbol2 = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
        fillSymbol2.style = Enums.SimpleFillSymbolStyleSolid;
        fillSymbol2.color = "green";
        var graphic2 = ArcGISRuntimeEnvironment.createObject("Graphic");
        graphic2.geometry = geometry2;
        graphic2.symbol = fillSymbol2;
        inputOverlay.graphics.append(graphic2);
    }

    function applyGeometryOperation(index) {
        if (map.loadStatus !== Enums.LoadStatusLoaded)
            return;

        // Perform geometry calculations
        var resultPolygon;
        switch (index) {
        case 1:
          resultPolygon = GeometryEngine.unionOf(geometry1, geometry2);
          break;
        case 2:
          resultPolygon = GeometryEngine.difference(geometry1, geometry2);
          break;
        case 3:
          resultPolygon = GeometryEngine.symmetricDifference(geometry1, geometry2);
          break;
        case 4:
          resultPolygon = GeometryEngine.intersection(geometry1, geometry2);
          break;
        case 0:
        default:
          break;
        }

        // Clear previous results
        outputOverlay.graphics.clear();
        if (!resultPolygon)
          return;

        // Add the resulting polygon as a Graphic
        var fillSymbol = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
        fillSymbol.style = Enums.SimpleFillSymbolStyleSolid;
        fillSymbol.color = "red";
        var graphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        graphic.geometry = resultPolygon;
        graphic.symbol = fillSymbol;
        outputOverlay.graphics.append(graphic);
    }
}
