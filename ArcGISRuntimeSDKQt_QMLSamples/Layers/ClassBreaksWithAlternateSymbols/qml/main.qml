// Copyright 2022 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.13
import Esri.ArcGISExtras 1.1

ApplicationWindow {
    id: appWindow
    width: 800
    height: 600
    title: "ClassBreaksWithAlternateSymbols"

    // add a mapView component
    MapView {
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true

        // add a map to the mapview
        Map {
            // add the ArcGISStreets basemap to the map
            initBasemapStyle: Enums.BasemapStyleArcGISStreets

            ArcGISMapImageLayer {
                url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    //TODO get the feature layer from the feature table and set the renderer fl.renderer
                    // Should add this as a Js function instead
                    var serviceFeatureTable = ArcGISRuntimeEnvironment.createObject("ServiceFeatureTable", {url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"});
                    featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: serviceFeatureTable});
                    featureLayer.loadStatusChanged.connect(function() {
                        if (featureLayer.loadStatus === Enums.LoadStatusLoaded) {
                            featureLayerAdded = true;
                        }
                    });

                    operationalLayers.append(featureLayer);
                    FeatureLayer.renderer = createClassBreaksRenderer_();
                }
            }

            ViewpointExtent {
                Envelope {
                    xMin: -229100
                    yMin: 6550700
                    xMax: -223300
                    yMax: 6552100
                    spatialReference: SpatialReference { wkid: 3857 }
                }
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
//                const extent = ArcGISRuntimeEnvironment.createObject("Envelope", {xMin: -229100, yMin: 6550700, xMax: -223300, yMax: 6552100, spatialReference: Factory.SpatialReference.createWebMercator()});
//                mapView.setViewpoint(ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: extent}));
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -229100, y: 6550700}), scale);
            }
        }

        Button {
            id: five_million_scale
            width: 200
            enabled: true
            text: "Set Scale to 5,000,000"
            onClicked:
            {
                const scale = 5000000;
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -229100, y: 6550700}), scale);
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
                mapView.setViewpointCenterAndScale(ArcGISRuntimeEnvironment.createObject("Point", {x: -229100, y: 6550700}), scale);
            }
        }
    }

    // Javascript code goes here
    function createClassBreaksRenderer_() {

        const orange_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_orange.png"})
        const multilayer_orange_tent = orange_tent.toMultilayerSymbol();
        multilayer_orange_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 0, maxScale: 10000000})

        //alternate symbol 1
        const red_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_red.png"})
        const multilayer_red_tent = red_tent.toMultilayerSymbol();
        multilayer_red_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 0, maxScale: 4000000})

        //alternate symbol 2
        const blue_tent = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {url: "qrc:/Samples/Layers/ClassBreaksWithAlternateSymbols/tent_blue.png"})
        const multilayer_blue_tent = blue_tent.toMultilayerSymbol();
        multilayer_blue_tent.referenceProperties = ArcGISRuntimeEnvironment.createObject("SymbolReferenceProperties", {minScale: 4000000, maxScale: 5000000})


        let class_break = ArcGISRuntimeEnvironment.createObject("ClassBreak", {label: "CB1", description: "CB1", minValue: 0, maxValue: 10000000, symbol: multilayer_orange_tent[0]});

        class_break.alternateSymbols.append(multilayer_red_tent);
        class_break.alternateSymbols.append(multilayer_blue_tent);

        //create the class breaks renderer using the class break created above
        const renderer = ArcGISRuntimeEnvironment.createObject("ClassBreaksRenderer");
        renderer.minValue = 0;
        renderer.defaultSymbol = multilayer_orange_tent;

        renderer.classBreaks.append(class_break);

        return renderer;
    }

    function setScale(){

    }
}
