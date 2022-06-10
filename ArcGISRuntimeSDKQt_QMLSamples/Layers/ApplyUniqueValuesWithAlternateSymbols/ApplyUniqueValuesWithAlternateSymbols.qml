// [WriteFile Name=ApplyUniqueValuesWithAlternateSymbols, Category=Layers]
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
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.15
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            FeatureLayer {
                id: featureLayer

                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"
                }
            }
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(initialViewpoint);
                }
                // Create the class breaks renderer with alternate symbols and assign it to the feature layer renderer
                featureLayer.renderer = createUniqueValueRenderer();
            }
        }
    }
    ViewpointCenter {
        id: initialViewpoint
        center: Point {
            x: -13631205.660131
            y: 4546829.846004
        }
        targetScale: 25000
    }
    Rectangle {
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .75
        radius: 5

        ColumnLayout {
            Text {
                color: "#ffffff"
                text: "Current scale: 1:" + Math.round(mapView.mapScale)
                Layout.fillWidth: true
                Layout.margins: 3
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
            }
            Button {
                text: qsTr("Reset Viewpoint")
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
                Layout.margins: 3
                Layout.fillWidth: true
                onClicked: mapView.setViewpointAndSeconds(initialViewpoint, 5);
            }
        }
    }
    function createUniqueValueRenderer() {
        // Create the default symbol
        const purpleDiamond = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                        color: "purple",
                                                                        size: 15,
                                                                        style: Enums.SimpleMarkerSymbolStyleDiamond
                                                                    });
        const purpleDiamondMultilayer = purpleDiamond.toMultilayerSymbol();

        const redTriangle = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                      color: "red",
                                                                      size: 30,
                                                                      style: Enums.SimpleMarkerSymbolStyleTriangle
                                                                  });
        const redTriangleMultilayer = redTriangle.toMultilayerSymbol();
        redTriangleMultilayer.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 5000, maxScale: 0});

        const alternateSymbols = createAlternateSymbols();

        const uniqueValue = ArcGISRuntimeEnvironment.createObject("UniqueValue", {label: "unique value", description: "unique value with alternate symbols", values: ["Damaged Property"], symbol: redTriangleMultilayer});

        uniqueValue.alternateSymbols.append(alternateSymbols[0]);
        uniqueValue.alternateSymbols.append(alternateSymbols[1]);

        // Create the unique value renderer using the class break created above
        const uniqueValueRenderer = ArcGISRuntimeEnvironment.createObject("UniqueValueRenderer");
        uniqueValueRenderer.defaultSymbol = purpleDiamondMultilayer;
        uniqueValueRenderer.fieldNames = ["req_type"];

        uniqueValueRenderer.uniqueValues.append(uniqueValue);

        return uniqueValueRenderer;
    }

    function createAlternateSymbols() {
        // Alternate symbol 1
        const blueSquare = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                     color: "blue",
                                                                     size: 45,
                                                                     style: Enums.SimpleMarkerSymbolStyleSquare
                                                                 });
        const blueSquareMultilayer = blueSquare.toMultilayerSymbol();
        blueSquareMultilayer.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 10000, maxScale: 5000});

        // Alternate symbol 2
        const yellowDiamond = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                        color: "yellow",
                                                                        size: 30,
                                                                        style: Enums.SimpleMarkerSymbolStyleDiamond
                                                                    });
        const yellowDiamondMultilayer = yellowDiamond.toMultilayerSymbol();
        yellowDiamondMultilayer.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 20000, maxScale: 10000});

        return [blueSquareMultilayer, yellowDiamondMultilayer];

    }
}
