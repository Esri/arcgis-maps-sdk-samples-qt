// [WriteFile Name=RasterRenderingRule, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.1
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property var renderingRuleNames: []
    property url imageServiceUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/CharlotteLAS/ImageServer"

    MapView {
        anchors.fill: parent
        id: mapView

        Map {
            id: map
            // create a basemap from a tiled layer and add to the map
            BasemapStreets {}

            // create and add a raster layer to the map
            RasterLayer {
                // create the raster layer from an image service raster
                ImageServiceRaster {
                    id: imageServiceRaster
                    url: imageServiceUrl

                    // zoom to the extent of the raster once it's loaded
                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            mapView.setViewpointGeometry(imageServiceRaster.serviceInfo.fullExtent);

                            var renderingRuleInfos = imageServiceRaster.serviceInfo.renderingRuleInfos;
                            for (var i = 0; i < renderingRuleInfos.length; i++) {
                                renderingRuleNames.push(renderingRuleInfos[i].name);
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                margins: 5 * scaleFactor
            }
            height: 80 * scaleFactor
            width: 200 * scaleFactor
            color: "silver"
            radius: 5 * scaleFactor

            Column {
                spacing: 10 * scaleFactor
                anchors.fill: parent
                anchors.margins: 5 * scaleFactor

                Label {
                    text: "Apply a Rendering Rule"
                }

                Row {
                    spacing: 5 * scaleFactor

                    ComboBox {
                        id: renderingRulesCombo
                        width: 130 * scaleFactor
                        model: renderingRuleNames
                    }

                    Button {
                        id: applyButton
                        text: "Apply"
                        width: 50 * scaleFactor
                        onClicked: {
                            applyRenderingRule(renderingRulesCombo.currentIndex);
                        }
                    }
                }
            }
        }
    }

    function applyRenderingRule(index) {
        // get the rendering rule info at the selected index
        var renderingRuleInfo = imageServiceRaster.serviceInfo.renderingRuleInfos[index];
        // create a rendering rule object using the rendering rule info
        var renderingRule = ArcGISRuntimeEnvironment.createObject("RenderingRule", {renderingRuleInfo: renderingRuleInfo});
        // create a new image service raster
        var newImageServiceRaster = ArcGISRuntimeEnvironment.createObject("ImageServiceRaster", {url: imageServiceUrl});
        // apply the rendering rule
        newImageServiceRaster.renderingRule = renderingRule;
        // create a raster layer using the image service raster
        var rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", {raster: newImageServiceRaster});
        // add the raster layer to the map
        map.operationalLayers.append(rasterLayer);
    }
}
