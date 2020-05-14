// [WriteFile Name=RasterLayerGeoPackage, Category=Layers]
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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/gpkg/"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapLightGrayCanvas {}

            // Load the GeoPackage once the Map loads
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    gpkg.load();
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    y: 39.7294
                    x: -104.8319
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 200000
            }
        }
    }

    // Declare a GeoPackage
    GeoPackage {
        id: gpkg
        path: dataPath + "AuroraCO.gpkg"

        // Wait for the GeoPackage to load successfully
        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            if (!gpkg.geoPackageRasters.length > 0)
                return;

            // Get the first raster
            const gpkgRaster = gpkg.geoPackageRasters[0];
            // Create a RasterLayer with the raster
            const rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", {raster: gpkgRaster});
            // Append the layer to the map
            map.operationalLayers.append(rasterLayer);
        }
    }
}
