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
                // create the class breaks renderer with alternate symbols and assign it to the feature layer renderer
                FeatureLayer.renderer = createClassBreaksRenderer_();
            }
        }
    }

    Column {
           anchors {
               left: parent.left
               top: parent.top
               margins: 15
           }
           spacing: 5

           Button {
               id: four_million_scale
               width: 200
               enabled: true
               text: "Set Scale to 4,000,000"
               onClicked:
               {
                   const scale = 4000000;
                   mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13496000, y: 4550700}), scale);
               }
           }

           Button {
               id: six_million_scale
               width: 200
               enabled: true
               text: "Set Scale to 6,000,000"
               onClicked:
               {
                   const scale = 6000000;
                   mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13496000, y: 4550700}), scale);
               }
           }

           Button {
               id: eight_million_scale
               width: 200
               enabled: true
               text: "Set Scale to 8,000,000"
               onClicked:
               {
                   const scale = 8000000;
                   mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -13496000, y: 4550700}), scale);
               }
           }
       }

       // Javascript code goes here
       function createClassBreaksRenderer_() {

           const orange_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_orange.png"})
           const multilayer_orange_tent = orange_tent.toMultilayerSymbol();
           multilayer_orange_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 0, maxScale: 4000000})

           //alternate symbol 1
           const red_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_red.png"})
           const multilayer_red_tent = red_tent.toMultilayerSymbol();
           multilayer_red_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 4000000, maxScale: 6000000})

           //alternate symbol 2
           const blue_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_blue.png"})
           const multilayer_blue_tent = blue_tent.toMultilayerSymbol();
           multilayer_blue_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 6000000, maxScale: 8000000})


           let class_break = ArcGISRuntimeEnvironment.createObject("ClassBreak", {label: "CB1", description: "CB1", minValue: 0, maxValue: 10000000, symbol: multilayer_orange_tent});

           class_break.alternateSymbols.append(multilayer_red_tent);
           class_break.alternateSymbols.append(multilayer_blue_tent);

           //create the class breaks renderer using the class break created above
           const renderer = ArcGISRuntimeEnvironment.createObject("ClassBreaksRenderer");
           renderer.minValue = 0;
           renderer.defaultSymbol = multilayer_orange_tent;

           renderer.classBreaks.append(class_break);

           return renderer;
       }
}
