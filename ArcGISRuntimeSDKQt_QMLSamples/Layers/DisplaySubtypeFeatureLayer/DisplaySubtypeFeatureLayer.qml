// [WriteFile Name=DisplaySubtypeFeatureLayer, Category=Layers]
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
import Esri.ArcGISRuntime 100.8

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapStreetsNightVector {}

            // create the feature layer
            SubtypeFeatureLayer  {
                id: featureLayer

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer/100"
                }
            }

            ViewpointExtent {
                extent: Envelope {
                    xMin: -9812691.11079696
                    yMin: 5128687.20710657
                    xMax: -9812377.9447607
                    yMax: 5128865.36767282
                }
            }
        }
    }
}
