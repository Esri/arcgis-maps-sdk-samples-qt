// [WriteFile Name=TimeBasedQuery, Category=Features]
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
            BasemapOceans {}

            FeatureLayer {
                ServiceFeatureTable {
                    id: hurricaneTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Hurricanes/MapServer/0"
                    featureRequestMode: Enums.FeatureRequestModeManualCache // set the cache mode to manual
                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            const clearCache = true;
                            const outFields = ["*"];

                            // populate the table with records matching the query
                            hurricaneTable.populateFromService(queryParams, clearCache, outFields);
                        }
                    }
                }
            }
        }
    }

    // create query parameters with time extent that includes anything before September 16, 2000
    QueryParameters {
        id: queryParams

        TimeExtent {
            startTime: new Date("January 1, 0001")
            endTime: new Date("September 16, 2000")
        }
    }
}
