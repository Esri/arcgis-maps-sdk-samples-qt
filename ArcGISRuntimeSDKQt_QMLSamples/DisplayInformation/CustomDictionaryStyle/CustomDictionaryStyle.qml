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
import Esri.ArcGISRuntime 100.11
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
            Basemap {
                initStyle: Enums.BasemapStyleArcGISStreets
            }

            // Set an initial viewpoint
            ViewpointCenter {
                Point {
                    x: -13046305
                    y: 4036698
                    SpatialReference { wkid: 3857 }
                }
                targetScale: 5000
            }

            // Add a Feature Layer to the Map
            FeatureLayer {
                id: restaurantsLayer
                ServiceFeatureTable {
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/rest/services/Redlands_Restaurants/FeatureServer/0"
                }

            }

            DictionaryRenderer {
                id: webDictionaryRenderer
                symbologyFieldOverrides: {"healthgrade": "inspection"}
                textFieldOverrides: {"healthgrade": "inspection"}

                Component.onCompleted: {
                    restaurantsLayer.renderer = webDictionaryRenderer;
                }

                DictionarySymbolStyle {
                    id: webDictionaryStyle
                    PortalItem {
                        itemId: "adee951477014ec68d7cf0ea0579c800"
                    }
                    autoLoad: true
                    onErrorChanged: {
                        console.log(error.message);
                    }
                    onLoadStatusChanged: {
                        console.log(loadStatus);
                    }
                }
            }

            // Set a Dictionary Renderer on the Feature Layer
            DictionaryRenderer {
                id: localDictionaryRenderer
                dictionarySymbolStyle: Factory.DictionarySymbolStyle.createFromFile(dataPath + "/styles/arcade_style/Restaurant.stylx")
            }

        }
    }
}
