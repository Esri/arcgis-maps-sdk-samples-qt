// [WriteFile Name=AddEncExchangeSet, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"
    property var loadedEncLayerCount: 0

    Component.onCompleted: {
        // set resource path
        EncEnvironmentSettings.resourcePath = dataPath + "/ENC/hydrography";

        // load the EncExchangeSet
        encExchangeSet.load();
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapOceans {}

            EncExchangeSet {
                id: encExchangeSet
                paths: [dataPath + "/ENC/ExchangeSetwithoutUpdates/ENC_ROOT/CATALOG.031"]

                property var layers: []

                // connect to the load status changed signal
                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusFailedToLoad) {
                        console.log("fail to load", error.message, error.additionalMessage);
                        return;
                    }

                    if (loadStatus !== Enums.LoadStatusLoaded) {
                        return;
                    }

                    // loop through the datasets
                    for (let i = 0; i < datasets.length; i++) {

                        // create an EncCell from each dataset
                        const encCell = ArcGISRuntimeEnvironment.createObject("EncCell", {
                                                                                  dataset: datasets[i]
                                                                              }, map);

                        // create an EncLayer from each cell
                        const encLayer = ArcGISRuntimeEnvironment.createObject("EncLayer", {
                                                                                   cell: encCell
                                                                               }, map);
                        layers.push(encLayer);

                        // connect to loadStatusChanged for each layer
                        encLayer.loadStatusChanged.connect(()=> {
                            if (encLayer.loadStatus === Enums.LoadStatusLoaded) {
                                loadedEncLayerCount++;
                            }

                            // loop through the layers and zoom to the combined full extent
                            if (loadedEncLayerCount === datasets.length) {
                                const fullExtents = [];
                                map.operationalLayers.forEach(layer => fullExtents.push(layer.fullExtent));
                                const fullExtentOfLayers = GeometryEngine.combineExtentsOfGeometries(fullExtents);
                                mapView.setViewpointGeometry(fullExtentOfLayers)
                            }
                        });

                        // add the layer to the map
                        map.operationalLayers.append(encLayer);
                    }
                }
            }
        }
    }
}
