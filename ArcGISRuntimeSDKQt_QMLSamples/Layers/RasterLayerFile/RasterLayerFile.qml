// [WriteFile Name=RasterLayerFile, Category=Layers]
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
    anchors.fill: parent
    
    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster/"
    readonly property var supportedFormats: ["img","I12","dt0","dt1","dt2","tc2","geotiff","tif", "tiff", "hr1","jpg","jpeg","jp2","ntf","png","i21","ovr"]
    property var rasterLayer: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map

            BasemapImagery {}

            //! [RasterLayerFile qml new raster layer]
            RasterLayer {
                Raster {
                    path: dataPath + "Shasta.tif"
                }
                // ...
                //! [RasterLayerFile qml new raster layer]

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    mapView.setViewpointCenterAndScale(fullExtent.center, 80000);
                }
            }
        }
    }

    Rectangle {
        visible: addButton.visible
        anchors.centerIn: addButton
        radius: 8
        height: addButton.height + (16)
        width: addButton.width + (16)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2
        opacity: 0.75
    }

    Button {
        id: addButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 32
        }

        text: "Add Raster"
        onClicked: loader.open();
    }

    RasterLoader {
        id: loader
        anchors.fill: rootRectangle
        folder: dataPath
        supportedExtensions: supportedFormats

        onRasterFileChosen: {
            createAndAddRasterLayer(url);
        }
    }

    function createAndAddRasterLayer(rasterUrl) {
        const newRaster = ArcGISRuntimeEnvironment.createObject("Raster", {path: rasterUrl});
        rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", {raster: newRaster});

        rasterLayer.loadStatusChanged.connect(zoomToRaster);

        map.operationalLayers.clear();
        map.operationalLayers.append(rasterLayer);
    }

    function zoomToRaster(){
        if (rasterLayer === null)
            return;

        if (rasterLayer.loadStatus !== Enums.LoadStatusLoaded)
            return;

        mapView.setViewpointCenterAndScale(rasterLayer.fullExtent.center, 80000);
    }
}
