// [WriteFile Name=Hillshade_Renderer, Category=Layers]
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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/data/raster"

    MapView {
        anchors.fill: parent

        Map {
            Basemap {
                // add a raster to the basemap
                RasterLayer {
                    Raster {
                        path: dataPath + "/srtm.tiff"
                    }

                    // declare a HillshadeRaster as a child of RasterLayer,
                    // as renderer is a default property of RasterLayer
                    HillshadeRenderer {
                        altitude: 45
                        azimuth: 315
                        zFactor: 0.000016
                        slopeType: Enums.SlopeTypeNone
                        pixelSizeFactor: 1
                        pixelSizePower: 1
                        outputBitDepth: 8
                    }
                }
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
