// [WriteFile Name=RasterFunctionService, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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
    
    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster/"

    MapView {
        anchors.fill: parent
        id: mapView

        Map {
            // create a basemap from a tiled layer and add to the map
            BasemapStreets {}

            // create and add a raster layer to the map
            RasterLayer {
                // create the raster layer from an image service raster
                ImageServiceRaster {
                    id: imageServiceRaster
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NLCDLandCover2001/ImageServer"

                    // zoom to the extent of the raster once it's loaded
                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            const scale = 50000000;
                            mapView.setViewpointCenterAndScale(imageServiceRaster.serviceInfo.fullExtent.center, scale);
                        }
                    }
                }
            }
        }
    }

    // Create a button to apply the raster function
    Rectangle {
        id: button
        property bool pressed: false

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20
            bottomMargin: 40
        }

        width: 45
        height: width
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 100
        border {
            color: "#585858"
            width: 1
        }

        Image {
            anchors {
                centerIn: parent
            }
            width: 26
            height: width
            source: "qrc:/Samples/Layers/RasterFunctionService/RasterImageAnalysisHillshade.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: button.pressed = true
            onReleased: button.pressed = false
            onClicked: applyRasterFunction()
        }
    }

    function applyRasterFunction() {
        if (!hillShadeJson.exists) {
            console.log("JSON file for the raster function does not exist");
            return;
        }

        //! [ImageServiceRaster Apply a raster function]
        // create raster function
        const rasterFunction = ArcGISRuntimeEnvironment.createObject("RasterFunction", {path: dataPath + "/hillshade_simplified.json"});
        // get the arguments
        const rasterFunctionArguments = rasterFunction.arguments;
        // set the raster
        rasterFunctionArguments.setRaster("raster", imageServiceRaster);

        // create the raster from the raster function
        const raster = ArcGISRuntimeEnvironment.createObject("Raster", { rasterFunction: rasterFunction });

        // create raster layer from raster
        const rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", { raster: raster });
        //! [ImageServiceRaster Apply a raster function]

        // add raster to map
        mapView.map.operationalLayers.clear();
        //! [ImageServiceRaster qml add raster operational]
        mapView.map.operationalLayers.append(rasterLayer);
        //! [ImageServiceRaster qml add raster operational]
    }

    FileInfo {
        id: hillShadeJson
        url: dataPath + "/hillshade_simplified.json"
    }
}
