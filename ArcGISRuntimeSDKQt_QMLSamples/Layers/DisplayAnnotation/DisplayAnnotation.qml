// [WriteFile Name=DisplayAnnotation, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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
            BasemapLightGrayCanvas {}

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://services1.arcgis.com/6677msI40mnLuuLr/arcgis/rest/services/East_Lothian_Rivers/FeatureServer/0"
                }
            }

            AnnotationLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/RiversAnnotation/FeatureServer/0"
                }
            }

            ViewpointCenter {
                // Specify the center Point
                Point {
                    x: -2.725610
                    y: 55.882436
                    SpatialReference { wkid: 4326 }
                }
                // Specify the scale
                targetScale: 50000
            }
        }
    }
}
