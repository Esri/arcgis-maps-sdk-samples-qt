// [WriteFile Name=DisplayWfsLayer, Category=Layers]
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

            FeatureLayer {
                // Create WFS Feature Table from Get Capabilities URL and table name
                WfsFeatureTable {
                    id: wfsFeatureTable
                    url: "https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities"
                    tableName: "Seattle_Downtown_Features:Buildings"

                    // Set feature request mode to manual - only manual is supported at v100.5.
                    // In this mode, you must manually populate the table - panning and zooming
                    // won't request features automatically.
                    featureRequestMode: Enums.FeatureRequestModeManualCache

                    // Once the table is loaded, request to populate the cache
                    onLoadStatusChanged: {
                        if (loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        populateWfsFeatureTable();
                    }
                }

                // Assign a renderer to the layer
                SimpleRenderer {
                    SimpleLineSymbol {
                        color: "red"
                        width: 3
                        style: Enums.SimpleLineSymbolStyleSolid
                    }
                }
            }

            // Set initial viewpoint on the map
            ViewpointExtent {
                Envelope {
                    id: initialExtent
                    xMin: -122.341581
                    yMin: 47.613758
                    xMax: -122.332662
                    yMax: 47.617207
                    spatialReference: SpatialReference { wkid: 4326 }
                }
            }
        }

        // Populate with more features once done navigating to new extent
        onNavigatingChanged: {
            if (navigating) {
                return;
            }

            populateWfsFeatureTable();
        }
    }

    function populateWfsFeatureTable() {
        // Create query parameters
        const params = ArcGISRuntimeEnvironment.createObject("QueryParameters", {
                                                                 geometry: mapView.visibleArea.extent,
                                                                 spatialRelationship: Enums.SpatialRelationshipIntersects
                                                             });

        // Populate features based on query
        wfsFeatureTable.populateFromService(params, false, ["*"]);
    }
}
