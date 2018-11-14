// [WriteFile Name=Unique_Value_Renderer, Category=DisplayInformation]
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

    // add a mapView component
    MapView {
        anchors.fill: parent

        // add map to the mapview
        Map {

            // add the topographic basemap to the map
            BasemapTopographic {}

            // create feature layer using service feature table
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"
                }

                // override the renderer of the feature layer with a new unique value renderer
                UniqueValueRenderer {
                    // set fields. Multiple fields can be set. In this sample, we only use one.
                    fieldNames: ["STATE_ABBR"]
                    defaultSymbol: SimpleFillSymbol {
                        style: Enums.SimpleFillSymbolStyleNull
                        color: "black"

                        SimpleLineSymbol {
                            style: Enums.SimpleLineSymbolStyleSolid
                            color: "grey"
                            width: 2
                        }
                    }

                    // set value for California
                    UniqueValue {
                        label: "California"
                        description: "The State of California"
                        values: ["CA"]

                        SimpleFillSymbol {
                            id: californiaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "red"

                            SimpleLineSymbol {
                                style: "SimpleLineSymbolStyleSolid"
                                color: "red"
                                width: 2
                            }
                        }
                    }

                    // set value for Arizona
                    UniqueValue {
                        label: "Arizona"
                        description: "The State of Arizona"
                        values: ["AZ"]

                        SimpleFillSymbol {
                            id: arizonaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "green"

                            SimpleLineSymbol {
                                style: "SimpleLineSymbolStyleSolid"
                                color: "green"
                                width: 2
                            }
                        }
                    }

                    // set value for Nevada
                    UniqueValue {
                        label: "Nevada"
                        description: "The State of Nevada"
                        values: ["NV"]

                        SimpleFillSymbol {
                            id: nevadaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "blue"

                            SimpleLineSymbol {
                                style: "SimpleLineSymbolStyleSolid"
                                color: "blue"
                                width: 2
                            }
                        }
                    }
                }
            }

            // set initial viewpoint
            ViewpointExtent {

                Envelope {
                    xMin: -13893029.0
                    yMin: 3573174.0
                    xMax: -12038972.0
                    yMax: 5309823.0
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }
    }
}
