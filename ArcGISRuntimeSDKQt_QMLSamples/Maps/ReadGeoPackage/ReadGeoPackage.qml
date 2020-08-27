// [WriteFile Name=ReadGeoPackage, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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
            initialViewpoint: vc
            BasemapTopographic {}
        }

        ViewpointCenter {
            id: vc
            center: Point {
                x: -104.8319
                y: 39.7294
                spatialReference: SpatialReference {
                    wkid: 4326
                }
            }

            targetScale: 500000
        }
    }

    // Load and read the GeoPackage
    GeoPackage {
        id: gpkg
        path: dataPath + "AuroraCO.gpkg"

        // Initial check to see if GeoPackage is loaded and not empty
        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            if (!gpkg.geoPackageRasters.length > 0 && !gpkg.geoPackageFeatureTables.length > 0)
                return;

            // For each raster, create a raster layer and add the layer to the map
            for (let k = 0; k < gpkg.geoPackageRasters.length; k++){
                const raster = gpkg.geoPackageRasters[k];
                const rasterLayer = ArcGISRuntimeEnvironment.createObject("RasterLayer", {raster: raster}, gpkg);
                rasterLayer.opacity = 0.55;
                map.operationalLayers.append(rasterLayer);
            }

            // For each feature table, create a feature layer and add the layer to the map
            for (let i = 0; i < gpkg.geoPackageFeatureTables.length; i++){
                const featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: gpkg.geoPackageFeatureTables[i]}, gpkg);
                map.operationalLayers.append(featureLayer);
            }
        }

        Component.onCompleted: load()
    }
}
