// [WriteFile Name=Simple_Marker_Symbol, Category=DisplayInformation]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    clip: true
    width: 800
    height: 600

    // Map view UI presentation at top
    MapView {

        anchors.fill: parent

        Map {
            BasemapImagery {}

            // create initial viewpoint
            ViewpointCenter {
                targetScale: 7500

                Point {
                    x: -226773
                    y: 6550477
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        // create a new GraphicsOverlay for the MapView
        GraphicsOverlay {

            // add graphic to overlay
            Graphic {

                // define position of graphic
                Point {
                    x: -226773
                    y: 6550477
                    spatialReference: SpatialReference { wkid: 3857 }
                }

                // set graphic to be rendered as a red circle symbol
                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleCircle
                    color: "red"
                    size: 12
                }
            }
        }
    }
}
