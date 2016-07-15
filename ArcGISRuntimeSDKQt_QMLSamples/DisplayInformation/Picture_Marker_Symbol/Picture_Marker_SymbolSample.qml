// [WriteFile Name=Picture_Marker_SymbolSample, Category=DisplayInformation]
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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.0

Rectangle {
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string inputPNG: "orange_symbol.png"
    property string outputPNG: System.temporaryFolder.path + "/" + inputPNG

    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            Envelope {
                id: envelope
                xMin: -228835
                xMax: -223560
                yMin: 6550763
                yMax: 6552021
                spatialReference: SpatialReference.createWebMercator()
            }

            // set initial viewpoint using envelope
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    mapView.setViewpointGeometryAndPadding(envelope, 200);
            }
        }

        GraphicsOverlay {

            // create Campsite Symbol from url and add to overlay
            Graphic {

                Point {
                    x: -228835
                    y: 6550763
                    spatialReference: SpatialReference.createWebMercator()
                }

                PictureMarkerSymbol {
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0/images/e82f744ebb069bb35b234b3fea46deae"
                }
            }

            // create blue symbol from local resource and add to overlay
            Graphic {

                Point {
                    x: -223560
                    y: 6552021
                    spatialReference: SpatialReference.createWebMercator()
                }

                PictureMarkerSymbol {
                    url: "qrc:/Samples/DisplayInformation/Picture_Marker_SymbolSample/blue_symbol.png"
                }
            }

            // create orange symbol from image on disk copied from resources
            Graphic {

                Point {
                    x: -226773
                    y: 6550477
                    spatialReference: SpatialReference.createWebMercator()
                }

                PictureMarkerSymbol {
                    url: copyOrangeSymbolImage(inputPNG)
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

    // TODO: figure out how to get this path to a resource
    FileFolder {
        id: folder
        path: "qrc:/Samples/DisplayInformation/Picture_Marker_SymbolSample/"
    }

    // delete temporary orange symbol image
    Component.onDestruction: {
        System.temporaryFolder.removeFile(inputPNG);
    }

    function copyOrangeSymbolImage(dataPath) {
        folder.copyFile(dataPath, outputPNG);
        return outputPNG;
    }
}
