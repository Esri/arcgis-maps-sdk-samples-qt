// [WriteFile Name=ChangeSublayerRenderer, Category=Layers]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property var sublayer: null
    property var originalRenderer: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapStreets {}

            ArcGISMapImageLayer {
                url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    // get the counties sublayer
                    sublayer = mapImageSublayers.get(2);

                    // get the original renderer once loaded
                    sublayer.loadStatusChanged.connect(()=> {
                        if (sublayer.loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        originalRenderer = sublayer.renderer;
                    });

                    // load the sublayer
                    sublayer.load();
                }
            }

            ViewpointExtent {
                Envelope {
                    xMin: -13834661.666904
                    yMin: 331181.323482
                    xMax: -8255704.998713
                    yMax: 9118038.075882
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }
    }

    // create class breaks renderer for the sublayer
    ClassBreaksRenderer {
        id: classBreaksRenderer
        fieldName: "POP2007"

        // declare the class breaks that make up the renderer, along with
        // a symbol for each class break
        ClassBreak {
            minValue: -99
            maxValue: 8560
            description: "-99 to 8560"
            label: description
            SimpleFillSymbol {
                color: Qt.rgba(0.89, 0.92, 0.81, 1)
                outline: SimpleLineSymbol {
                    id: outlineSymbol
                    color: Qt.rgba(0.6, 0.6, 0.6, 1)
                    width: 1
                }
                style: Enums.SimpleFillSymbolStyleSolid
            }
        }

        ClassBreak {
            minValue: 8560
            maxValue: 18109
            description: "> 8,560 to 18,109"
            label: description
            SimpleFillSymbol {
                color: Qt.rgba(0.59, 0.76, 0.75, 1)
                outline: outlineSymbol
                style: Enums.SimpleFillSymbolStyleSolid
            }
        }

        ClassBreak {
            minValue: 18109
            maxValue: 35501
            description: "> 18,109 to 35,501"
            label: description
            SimpleFillSymbol {
                color: Qt.rgba(0.38, 0.65, 0.71, 1)
                outline: outlineSymbol
                style: Enums.SimpleFillSymbolStyleSolid
            }
        }

        ClassBreak {
            minValue: 35501
            maxValue: 86100
            description: "> 35,501 to 86,100"
            label: description
            SimpleFillSymbol {
                color: Qt.rgba(0.27, 0.49, 0.59, 1)
                outline: outlineSymbol
                style: Enums.SimpleFillSymbolStyleSolid
            }
        }

        ClassBreak {
            minValue: 86100
            maxValue: 10110975
            description: "> 86,100 to 10,110,975"
            label: description
            SimpleFillSymbol {
                color: Qt.rgba(0.16, 0.33, 0.47, 1)
                outline: outlineSymbol
                style: Enums.SimpleFillSymbolStyleSolid
            }
        }
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            margins: 5
        }
        spacing: 5

        Button {
            width: 200
            text: "Apply Renderer"
            enabled: sublayer ? sublayer.loadStatus === Enums.LoadStatusLoaded : false
            onClicked: sublayer.renderer = classBreaksRenderer;
        }

        Button {
            width: 200
            text: "Reset"
            enabled: sublayer ? sublayer.loadStatus === Enums.LoadStatusLoaded : false
            onClicked: sublayer.renderer = originalRenderer;
        }
    }
}
