// [WriteFile Name=RasterColormapRenderer, Category=Layers]
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

    //! [RasterColormapRenderer qml add raster basemap]
    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapImagery {}

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                setColormap();
                operationalLayers.append(rasterLayer);
            }
        }
    }

    RasterLayer {
        id: rasterLayer

        Raster {
            path: dataPath + "/ShastaBW.tif"
        }

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            mapView.setViewpointGeometryAndPadding(fullExtent, 50);
        }   
    }

    function setColormap() {

        // create a color map where values 0-150 are red and 150-250 are yellow
        const colorArray = [];
        for (let i = 0; i < 250; i++)
            colorArray.push(i < 150 ? Qt.rgba(255, 0, 0) : Qt.rgba(255,255,0));

        // create a colormap
        const colormap = ArcGISRuntimeEnvironment.createObject("Colormap", {colors: colorArray});

        // create colormap renderer
        const colormapRenderer = ArcGISRuntimeEnvironment.createObject("ColormapRenderer", {colormap: colormap});

        // apply the renderer to the raster layer
        rasterLayer.renderer = colormapRenderer;
    }
    //! [RasterColormapRenderer qml add raster basemap]
}
