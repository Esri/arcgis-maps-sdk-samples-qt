// [WriteFile Name=ShowLabelsOnLayers, Category=DisplayInformation]
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
import Esri.ArcGISRuntime 100.5

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
                // Add a feature service
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/1"
                }

                // Enable Labels
                labelsEnabled: true

                // Add a label definition to define labels for the layer
                //
                // Help regarding the JSON syntax for defining the LabelDefinition.FromJson syntax can be found here:
                // https://developers.arcgis.com/web-map-specification/objects/labelingInfo/
                // This particular JSON string will have the following characteristics:
                // (1) The 'labelExpressionInfo' defines that the label text displayed comes from the field 'rte_num1' in the
                //     feature service and will be prefaced with an "I -". Example: "I - 10", "I - 15", "I - 95", etc.
                // (2) The 'labelPlacement' will be placed above and along the highway polyline segment.
                // (3) The 'where' clause restricts the labels to be displayed that has valid (non-empty) data. Empty data
                //     for this service has a single blank space in the 'rte_num1' field.
                // (4) The 'symbol' for the labeled text will be blue with a yellow halo.
                LabelDefinition {
                    json: {
                        "labelExpressionInfo":{
                            "expression":"'I - ' + $feature.rte_num1"
                        },
                        "labelPlacement":"esriServerLinePlacementAboveAlong",
                        "where":"rte_num1 <> ' '",
                        "symbol": {
                            "angle":0,
                            "backgroundColor":[0,0,0,0],
                            "borderLineColor":[0,0,0,0],
                            "borderLineSize":0,
                            "color":[0,0,255,255],
                            "font": {
                                "decoration":"none",
                                "size":15,
                                "style":"normal",
                                "weight":"normal"
                            },
                            "haloColor":[255,255,0,255],
                            "haloSize":1.5,
                            "horizontalAlignment":"center",
                            "kerning":false,
                            "type":"esriTS",
                            "verticalAlignment":"middle",
                            "xoffset":0,
                            "yoffset":0
                        }
                    }
                }

                // zoom to the layer once it loads
                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded)
                        mapView.setViewpointCenterAndScale(fullExtent.center, 56759600);
                }
            }
        }
    }
}
