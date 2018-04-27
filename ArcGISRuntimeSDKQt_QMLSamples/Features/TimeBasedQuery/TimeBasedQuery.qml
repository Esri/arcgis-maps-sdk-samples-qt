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
import Esri.ArcGISRuntime 100.3

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
                    onPopulateFromServiceStatusChanged: {
                        if (populateFromServiceStatus === Enums.TaskStatusCompleted) {
                            console.log("complete")
                        }
                        else if (populateFromServiceStatus === Enums.TaskStatusInProgress)
                            console.log("in progress")
                        else if (populateFromServiceStatus === Enums.TaskStatusErrored)
                            console.log("error", error.message, error.additionalMessage)
                    }
                    onErrorChanged: {
                        if (error)
                            console.log("error", error.message, error.additionalMessage)
                    }
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    var clearCache = true;
                    var outFields = ["*"];
                    hurricaneTable.populateFromService(queryParams, clearCache, outFields);
                }
            }
        }
    }

    QueryParameters {
        id: queryParams

        TimeExtent {
            startTime: new Date("January 1, 1900")
            endTime: new Date("September 16, 2000")
        }
        Component.onCompleted: console.log("start time", timeExtent.startTime, "end time", timeExtent.endTime)
    }
}
