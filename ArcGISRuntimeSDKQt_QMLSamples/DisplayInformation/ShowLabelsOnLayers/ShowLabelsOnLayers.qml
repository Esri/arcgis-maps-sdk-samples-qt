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
import Esri.ArcGISRuntime 100.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISLightGray
            }

            FeatureLayer {
                // Add a feature service
                ServiceFeatureTable {
                    url: "https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/USA_115th_Congressional_Districts/FeatureServer/0"
                }

                // Enable Labels
                labelsEnabled: true

                // Add a label definition to define labels for the layer
                //
                // (1) The 'ArcadeLabelExpression' defines that the label text displayed comes from the fields 'NAME',
                //     'PARTY' (R or D), and 'CDFIPS' in the feature service in the format
                //     "Firstname Lastname (R or D)
                //     District #".
                // (2) The 'textSymbol' for the labeled text will be red (for Republican) or blue (for Democrat) with a white halo, in the center of the polygon.
                // (3) The 'whereClause' restricts the labels to data from Republican or Democrat districts.

                LabelDefinition {
                    id: republicanJson

                    expression: ArcadeLabelExpression {
                        expression: "$feature.NAME + ' (' + left($feature.PARTY,1) + ') \\nDistrict ' + $feature.CDFIPS"
                    }

                    textSymbol: TextSymbol {
                        color: "red"
                        haloColor: "white"
                        haloWidth: 1.5
                        size: 8
                        horizontalAlignment: Enums.HorizontalAlignmentCenter
                        verticalAlignment: Enums.VerticalAlignmentMiddle
                    }

                    whereClause: "PARTY = 'Republican'"
                }

                LabelDefinition {
                    id: democratJson

                    expression: ArcadeLabelExpression {
                        expression: "$feature.NAME + ' (' + left($feature.PARTY,1) + ') \\nDistrict ' + $feature.CDFIPS"
                    }

                    textSymbol: TextSymbol {
                        color: "blue"
                        haloColor: "white"
                        haloWidth: 1.5
                        size: 8
                        horizontalAlignment: Enums.HorizontalAlignmentCenter
                        verticalAlignment: Enums.VerticalAlignmentMiddle
                    }

                    whereClause: "PARTY = 'Democrat'"
                }

                LabelDefinition {
                    id: democratJson2
                    json: {
                        "labelExpressionInfo":{
                            "expression":"$feature.NAME + ' (' + left($feature.PARTY,1) + ') \\nDistrict ' + $feature.CDFIPS"
                        },
                        "labelPlacement":"esriServerPolygonPlacementAlwaysHorizontal",
                        "where":"PARTY = 'Democrat'",
                        "symbol": {
                            "angle":0,
                            "backgroundColor":[0,0,0,0],
                            "borderLineColor":[0,0,0,0],
                            "borderLineSize":0,
                            "color":[0,0,255,255],
                            "font": {
                                "decoration":"none",
                                "size":8,
                                "style":"normal",
                                "weight":"normal"
                            },
                            "haloColor":[255,255,255,255],
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
