// [WriteFile Name=FeatureLayer_ChangeRenderer, Category=Features]
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

    

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            BasemapTopographic {}

            // create the feature layer
            FeatureLayer {
                id: featureLayer

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/PoolPermits/FeatureServer/0"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(viewPoint);
                }
            }
        }

        SimpleRenderer {
            id: renderer

            SimpleLineSymbol {
                style: Enums.SimpleLineSymbolStyleSolid
                color: "blue"
                antiAlias: true
                width: 2
            }
        }

        ViewpointExtent {
            id: viewPoint
            extent: Envelope {
                xMin: -13075816.4047166
                yMin: 4014771.46954516
                xMax: -13073005.6797177
                yMax: 4016869.78617381
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
        }
    }

    Row {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 5
            bottomMargin: 25
        }
        spacing: 5

        // button to change renderer
        Button {
            text: "Change Renderer"
            enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
            onClicked: {
                featureLayer.renderer = renderer;
            }
        }
    }
}
