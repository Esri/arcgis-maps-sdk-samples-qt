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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    
    MapView {
        id: mapView
        anchors.fill: parent

        //! [HillshadeRenderer QML apply to layer snippet]
        Map {
            Basemap {
                // add a raster to the basemap
                RasterLayer {
                    id: rasterLayer

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
            //! [HillshadeRenderer QML apply to layer snippet]

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpointScale(754479);
                }
            }
        }
    }

    Button {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 25
        }
        text: "Edit Renderer"
        onClicked: hillshadeSettings.visible = true;
    }

    HillshadeSettings {
        id: hillshadeSettings
        anchors.fill: parent
    }

    function applyHillshadeRenderer(altitude, azimuth, slope) {
        // create the new renderer
        const hillshadeRenderer = ArcGISRuntimeEnvironment.createObject("HillshadeRenderer", {
                                                                            altitude: altitude,
                                                                            azimuth: azimuth,
                                                                            zFactor: 0.000016,
                                                                            slopeType: slope,
                                                                            pixelSizeFactor: 1,
                                                                            pixelSizePower: 1,
                                                                            outputBitDepth: 8
                                                                        });

        // set the renderer on the layer
        rasterLayer.renderer = hillshadeRenderer;
    }
}
