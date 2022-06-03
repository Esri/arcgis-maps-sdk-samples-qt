// [WriteFile Name=ClassBreaksWithAlternateSymbols, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.13
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            // create the feature layer
            FeatureLayer {
                id: featureLayer

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"
                }
            }
            onLoadStatusChanged: {

                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(initialViewpoint);
                }

                // create the class breaks renderer with alternate symbols and assign it to the feature layer renderer
                featureLayer.renderer = createClassBreaksRenderer_();
            }



        }
    }

    ViewpointCenter {
            id: initialViewpoint
            center: Point {
                x: -13632095.660131
                y: 4545009.846004
            }
            targetScale: 50000
        }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        spacing: 5

        Button {
            width: 200
            enabled: true
            text: "Set Scale to 2500"
            onClicked:
            {
                const scale = 2500;
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13632095.660131, y: 4545009.846004}), scale);
            }
        }

        Button {
            width: 200
            enabled: true
            text: "Set Scale to 7500"
            onClicked:
            {
                const scale = 7500;
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13632095.660131, y: 4545009.846004}), scale);
            }
        }

        Button {
            width: 200
            enabled: true
            text: "Set Scale to 15000"
            onClicked:
            {
                const scale = 15000;
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13632095.660131, y: 4545009.846004}), scale);
            }
        }
    }

    // Javascript code goes here
    function createClassBreaksRenderer_() {


        // create the default symbol
        const purple_diamond = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                             color: "purple",
                                                                             size: 30,
                                                                             style: Enums.SimpleMarkerSymbolStyleDiamond
                                                                         });
        const purple_diamond_ml = purple_diamond.toMultilayerSymbol();
        purple_diamond_ml.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 5000, maxScale: 0});

        const red_triangle = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                             color: "red",
                                                                             size: 30,
                                                                             style: Enums.SimpleMarkerSymbolStyleTriangle
                                                                         });
        const red_triangle_ml = red_triangle.toMultilayerSymbol();
        red_triangle_ml.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 5000, maxScale: 0});

        const alternate_symbols = createAlternateSymbols_();

        let class_break = ArcGISRuntimeEnvironment.createObject("ClassBreak", {label: "CB1", description: "CB1", minValue: 0, maxValue: 1, symbol: red_triangle_ml});

        class_break.alternateSymbols.append(alternate_symbols[0]);
        class_break.alternateSymbols.append(alternate_symbols[1]);

        //create the class breaks renderer using the class break created above
        const class_breaks_renderer = ArcGISRuntimeEnvironment.createObject("ClassBreaksRenderer");
        class_breaks_renderer.minValue = 0;
        class_breaks_renderer.defaultSymbol = purple_diamond_ml;
        class_breaks_renderer.fieldName = "status";

        class_breaks_renderer.classBreaks.append(class_break);

        return class_breaks_renderer;
    }

    function createAlternateSymbols_()
    {
        // alternate symbol 1
        const blue_square = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                             color: "blue",
                                                                             size: 30,
                                                                             style: Enums.SimpleMarkerSymbolStyleSquare
                                                                         });
        const blue_square_ml = blue_square.toMultilayerSymbol();
        blue_square_ml.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 10000, maxScale: 5000});

        // alternate symbol 2
        const yellow_diamond = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                             color: "yellow",
                                                                             size: 30,
                                                                             style: Enums.SimpleMarkerSymbolStyleDiamond
                                                                         });
        const yellow_diamond_ml = yellow_diamond.toMultilayerSymbol();
        yellow_diamond_ml.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 20000, maxScale: 10000});

        return [blue_square_ml, yellow_diamond_ml];

    }
}
