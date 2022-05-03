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
import Esri.ArcGISRuntime 100.15
import Esri.ArcGISExtras 1.1
import QtQuick.Controls 2.2

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            // Set basemap
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
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
                // Set initial FeatureLayer renderer to the local DictionaryRenderer
                renderer: localDictionaryRenderer
                property bool usingLocalRenderer: true

            }

            // Create a DictionaryRenderer using the local .stylx file
            DictionaryRenderer {
                id: localDictionaryRenderer
                dictionarySymbolStyle: Factory.DictionarySymbolStyle.createFromFile(dataPath + "/styles/arcade_style/Restaurant.stylx")
            }

            DictionaryRenderer {
                id: webDictionaryRenderer

                // The source feature layer fields do not match those of the the DictionarySymbolStyle
                // With the following overrides, the feature layer's "inspection" field will be mapped to the dictionary symbol style's "healthgrade" field
                symbologyFieldOverrides: {"healthgrade": "inspection"}
                textFieldOverrides: {"healthgrade": "inspection"}

                // Create a DictionarySymbolStyle from a portal item, using the default arcgis.com path
                DictionarySymbolStyle {
                    portalItem: PortalItem {
                        itemId: "adee951477014ec68d7cf0ea0579c800"
                    }
                }
            }
        }
    }

    Rectangle {
        id: rectangle
        anchors {
            left: parent.left
            top: parent.top
            margins: 5
        }
        width: radioColumn.width
        height: radioColumn.height
        color: "white"
        border {
            color: "black"
            width: 1
        }
        opacity: 0.9

        Column {
            id: radioColumn
            padding: 5
            spacing: 5

            Text {
                text: "Custom Dictionary Symbol Style Source"
            }

            RadioButton {
                text: "Local .stylx file"
                checked: true
            }

            RadioButton {
                text: "Web style"
                onCheckedChanged: {
                    restaurantsLayer.renderer = checked ? webDictionaryRenderer : localDictionaryRenderer;
                }
            }
        }
    }
}
