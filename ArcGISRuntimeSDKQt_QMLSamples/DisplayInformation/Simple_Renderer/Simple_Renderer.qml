// [WriteFile Name=Simple_Renderer, Category=DisplayInformation]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    clip: true
    width: 800
    height: 600

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapImagery {}

            Envelope {
                id: envelope
                xMin: -110.828140
                yMin: 44.460458
                xMax: -110.829381
                yMax: 44.462735
                spatialReference: Factory.SpatialReference.createWgs84()
            }

            // set initial Viewpoint
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    mapView.setViewpointGeometryAndPadding(envelope, 50);
            }
        }

        // create graphic overlay
        GraphicsOverlay {

            // set renderer for overlay
            SimpleRenderer {

                // set symbol as red cross
                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleCross
                    color: "red"
                    size: 12
                }
            }

            // add the points to be rendered
            Graphic {

                // Old Faithful
                Point {
                    x: -110.828140
                    y: 44.460458
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
            }

            Graphic {

                // Cascade Geyser
                Point {
                    x: -110.829004
                    y: 44.462438
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
            }

            Graphic {

                // Plume Geyser
                Point {
                    x: -110.829381
                    y: 44.462735
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
            }
        }
    }
}
