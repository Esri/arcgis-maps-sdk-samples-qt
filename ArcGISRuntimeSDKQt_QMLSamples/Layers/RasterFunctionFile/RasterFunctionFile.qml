// [WriteFile Name=RasterFunctionFile, Category=Layers]
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
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: 1
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    property url rasterPath: dataPath + "/Shasta_Elevation.tif"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapImagery {}

            RasterLayer {
                id: rasterLayer
                opacity: 0.5

                Raster {
                    id: theRaster
                    path: rasterPath
                }
            }

            ViewpointExtent {
                Envelope {
                    json: {
                        "spatialReference": {
                            "latestWkid": 3857,
                            "wkid": 102100
                        },
                        "xmax": -13591503.517810356,
                        "xmin": -13606233.44023646,
                        "ymax": 4982810.138527592,
                        "ymin": 4971762.696708013
                    }
                }
            }
        }
    }

    // Create a button to apply the raster function
    Rectangle {
        id: button
        enabled: theRaster.loadStatus === Enums.LoadStatusLoaded
        property bool pressed: false

        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20 * scaleFactor
            bottomMargin: 40 * scaleFactor
        }

        width: 45 * scaleFactor
        height: width
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 100
        border {
            color: "#585858"
            width: 1 * scaleFactor
        }

        Image {
            anchors {
                centerIn: parent
                verticalCenterOffset: -3 * scaleFactor
            }
            width: 26 * scaleFactor
            height: width
            source: "qrc:/Samples/Layers/RasterFunctionFile/color.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: button.pressed = true
            onReleased: button.pressed = false
            onClicked: applyRasterFunction()
        }
    }

    //! [RasterFunctionFile qml raster from function]
    function applyRasterFunction() {
        // create raster function
        var rasterFunction = createRasterFunction();

        // check for valid raster function
        if (!rasterFunction)
            return;

        // create the raster from the raster function
        var raster = ArcGISRuntimeEnvironment.createObject("Raster", { rasterFunction: rasterFunction });

        // create raster layer from raster
        var newRasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", { raster: raster });
        newRasterLayer.opacity = 0.5;

        // add raster to map
        mapView.map.operationalLayers.clear();
        mapView.map.operationalLayers.append(newRasterLayer);
    }

    function createRasterFunction() {
        // Check if the raster function json exists
        if (!colorJson.exists)
          return;

        // create the raster function
        var rasterFunction = ArcGISRuntimeEnvironment.createObject("RasterFunction", {path: dataPath + "/color.json"});

        // check for valid raster function
        if (!rasterFunction)
          return;

        rasterFunction.arguments.setRaster("raster", theRaster);
        rasterFunction.arguments.setRaster("raster", theRaster);

        return rasterFunction;
    }

    FileInfo {
        id: colorJson
        url: dataPath + "/color.json"
    }
    //! [RasterFunctionFile qml raster from function]
}
