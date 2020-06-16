// [WriteFile Name=CustomDictionaryStyle, Category=DisplayInformation]
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

    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            // Set basemap
            BasemapStreetsVector{}

            // Set an initial viewpoint
            ViewpointCenter {
                Point {
                    x: -1.304630524635E7
                    y: 4036698.1412000023
                    SpatialReference { wkid: 3857 }
                }
                targetScale: 5000
            }

            // Add a Feature Layer to the Map
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Redlands_Restaurants/FeatureServer/0"
                }

                // Set a Dictionary Renderer on the Feature Layer
                DictionaryRenderer {
                    id: dictionaryRenderer
                    dictionarySymbolStyle: Factory.DictionarySymbolStyle.createFromFile(dataPath + "/styles/arcade_style/Restaurant.stylx")
                }
            }
        }
    }
}
