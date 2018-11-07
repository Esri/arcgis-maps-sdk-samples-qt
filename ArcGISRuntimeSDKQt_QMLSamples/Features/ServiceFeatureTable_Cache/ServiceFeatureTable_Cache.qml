// [WriteFile Name=ServiceFeatureTable_Cache, Category=Features]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            BasemapLightGrayCanvas {}

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/PoolPermits/FeatureServer/0"
                    featureRequestMode: Enums.FeatureRequestModeOnInteractionCache
                }
            }
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(viewPoint);
                }
            }
        }

        ViewpointExtent {
            id: viewPoint
            extent: Envelope {
                xMin: -1.30758164047166E7
                yMin: 4014771.46954516
                xMax: -1.30730056797177E7
                yMax: 4016869.78617381
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
        }
    }
}
