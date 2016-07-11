
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent

        // add map to the mapview
        Map {

            // add the topographic basemap to the map
            BasemapTopographic {}

            // create feature layer using service feature table
            FeatureLayer {
                id: featureLayer

                ServiceFeatureTable {
                    id: featureTable
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"
                }

                // override the renderer of the feature layer with a new unique value renderer
                UniqueValueRenderer {
                    id: uniqueValueRenderer
                    fieldNames: ["STATE_NAME"]
                    defaultSymbol: SimpleFillSymbol {
                        style: Enums.SimpleFillSymbolStyleNull
                        color: "black"
                        outline: SimpleLineSymbol {
                            style: "SimpleLineSymbolStyleSolid"
                            color: "grey"
                            width: 2
                        }
                    }

                    // set value for CA
                    UniqueValue {
                        label: "California"
                        description: "The State of California"
                        values: ["California"]
                        symbol: SimpleFillSymbol {
                            id: californiaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "red"
                            outline: SimpleLineSymbol {
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
                        values: ["Arizona"]
                        symbol: SimpleFillSymbol {
                            id: arizonaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "green"
                            outline: SimpleLineSymbol {
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
                        values: ["Nevada"]
                        symbol: SimpleFillSymbol {
                            id: nevadaFillSymbol
                            style: Enums.SimpleFillSymbolStyleSolid
                            color: "blue"
                            outline: SimpleLineSymbol {
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
                id: viewPoint
                extent: Envelope {
                    xMin: -13893029.0
                    yMin: 3573174.0
                    xMax: -12038972.0
                    yMax: 5309823.0
                    spatialReference: SpatialReference.createWebMercator()
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
