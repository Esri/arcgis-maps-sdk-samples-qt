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
import Esri.ArcGISRuntime 100.6
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    Component.onCompleted: {
        //EncEnvironmentSettings.sencDataPath
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapOceans {}

            EncExchangeSet {
                paths: [dataPath + "/ENC/ExchangeSetwithoutUpdates/ENC_ROOT/CATALOG.031"]

                // load the EncExchangeSet
                Component.onCompleted: load();

                // connect to the load status changed signal
                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    // create full extent variable
                    var fullExtent;
                    var count = 0;

                    // loop through the datasets
                    for (var i = 0; i < datasets.length; i++) {

                        // create an EncCell from each dataset
                        var encCell = ArcGISRuntimeEnvironment.createObject("EncCell", {
                                                                                dataset: datasets[i]
                                                                            });

                        // create an EncLayer from each cell
                        var encLayer = ArcGISRuntimeEnvironment.createObject("EncLayer", {
                                                                                 cell: encCell
                                                                             });

                        encLayer.loadStatusChanged.connect(function() {
                            if (encLayer.loadStatus === Enums.LoadStatusLoaded)
                                count++;

                            if (count === datasets.length) {
                                for (var index = 0; index < map.operationalLayers.count; index++) {
                                    var lyr = map.operationalLayers.get(index);
                                    if (index === 0)
                                        fullExtent = lyr.fullExtent;

                                    fullExtent = GeometryEngine.combineExtents(fullExtent, lyr.fullExtent);
                                    mapView.setViewpointGeometry(fullExtent)
                                }
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
