// [WriteFile Name=DisplayFeatureLayers, Category=Features]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.ArcGISRuntime
import Esri.ArcGISExtras


Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: {
        Qt.platform.os === "ios" ?
                    System.writableLocationUrl(System.StandardPathsDocumentsLocation) + "/ArcGIS/Runtime/Data/" :
                    System.writableLocationUrl(System.StandardPathsHomeLocation) + "/ArcGIS/Runtime/Data/"
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -13214155
                    y: 4040194
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 35e4
            }
        }
    }

    Rectangle {
        id: layerSelectRectangle
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        width: 150
        height: column.height + 10
        border.color: "black"

        ColumnLayout {
            id: column
            anchors.centerIn: parent
            Text {
                text: "Feature Layer Mode"
            }
            ComboBox {
                id: featureLayerComboBox
                implicitWidth: layerSelectRectangle.width - 10
                model: ["Geodatabase", "GeoPackage", "Portal Item", "Service Feature Table", "Shapefile"]
                onCurrentTextChanged: {
                    map.operationalLayers.clear();

                    switch (currentText) {
                    case "Geodatabase":
                        setGeodatabaseLayer();
                        break;
                    case "GeoPackage":
                        setGeoPackageLayer();
                        break;
                    case "Portal Item":
                        setPortalItemLayer();
                        break;
                    case "Service Feature Table":
                        setServiceFeatureTableLayer();
                        break;
                    case "Shapefile":
                        setShapefileLayer();
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

    // Display a feature layer with a Geodatabase
    FeatureLayer {
        id: gdbFeatureLayer
        featureTable: geodatabase.geodatabaseFeatureTablesByTableName["Trailheads"] ?? null
        Geodatabase {
            id: geodatabase
            path: dataPath + "geodatabase/LA_Trails.geodatabase"
        }
    }

    function setGeodatabaseLayer() {
        map.operationalLayers.append(gdbFeatureLayer);

        // Set the viewpoint to the layer
        const viewpointCenterPoint = ArcGISRuntimeEnvironment.createObject("Point", {x: -13214155, y: 4040194, spatialReference: Factory.SpatialReference.createWebMercator()});
        mapView.setViewpointCenterAndScale(viewpointCenterPoint, 35e4);
    }

    // Display a GeoPackage feature layer
    FeatureLayer {
        id: gpkgFeatureLayer
    }

    // Declare a GeoPackage
    GeoPackage {
        id: gpkg
        path: dataPath + "gpkg/AuroraCO.gpkg"

        // Wait for the GeoPackage to load successfully
        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded || // the GeoPackage hasn't yet loaded
                    featureLayerComboBox.currentText !== "GeoPackage" || // the user selected a different option while loading and this process should be aborted
                    gpkg.geoPackageFeatureTables.length === 0) // the GeoPackage has no feature tables to use
                return;

            setGeoPackageLayer();
        }

        onErrorChanged: {
            console.log(error.message, error.additionalMessage);
        }
    }

    function setGeoPackageLayer() {
        if (gpkg.loadStatus !== Enums.LoadStatusLoaded) {
            // The GeoPackage must be explicitly loaded before its feature tables can be accessed
            gpkg.load();
            return;
        }

        // A GeoPackageFeatureTable can only be accessed by one feature layer at a time
        // So we set this feature table on a persistent layer in case the user wants to view it again
        if (!gpkgFeatureLayer.featureTable) {
            // Get the first feature table
            const gpkgFeatureTable = gpkg.geoPackageFeatureTables[0];
            // Create a FeatureLayer with the table
            gpkgFeatureLayer.featureTable = gpkgFeatureTable;
        }

        // Append the layer to the map
        map.operationalLayers.append(gpkgFeatureLayer);

        // Set the viewpoint to the layer
        const viewpointCenterPoint = ArcGISRuntimeEnvironment.createObject("Point", {x: -104.8319, y: 39.7294, spatialReference: Factory.SpatialReference.createWgs84()});
        mapView.setViewpointCenterAndScale(viewpointCenterPoint, 2e5);
    }

    // Display a feature layer with a portal item
    FeatureLayer {
        id: portalItemFeatureLayer
        item: PortalItem {
            itemId: "1759fd3e8a324358a0c58d9a687a8578"
        }
    }

    function setPortalItemLayer() {
        map.operationalLayers.append(portalItemFeatureLayer);

        // Set the viewpoint to the layer
        const viewpointCenterPoint = ArcGISRuntimeEnvironment.createObject("Point", {x: -122.6219, y: 45.5266, spatialReference: Factory.SpatialReference.createWgs84()});
        mapView.setViewpointCenterAndScale(viewpointCenterPoint, 6000);
    }

    // Display a service feature table feature layer
    FeatureLayer {
        id: serviceFeatureTableFeatureLayer
        featureTable: ServiceFeatureTable {
            url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"
        }
    }

    function setServiceFeatureTableLayer() {
        map.operationalLayers.append(serviceFeatureTableFeatureLayer);

        // Set the viewpoint to the layer
        const viewpointCenterPoint = ArcGISRuntimeEnvironment.createObject("Point", {x: -13176752, y: 4090404, spatialReference: Factory.SpatialReference.createWebMercator()});
        mapView.setViewpointCenterAndScale(viewpointCenterPoint, 300000);
    }

    // Display a shapefile feature layer
    FeatureLayer {
        id: shapefileFeatureLayer
        featureTable: ShapefileFeatureTable {
            path: dataPath + "shp/Public_Art.shp"
        }
    }

    function setShapefileLayer() {
        map.operationalLayers.append(shapefileFeatureLayer);

        // Set the viewpoint to the layer
        const viewpointCenterPoint = ArcGISRuntimeEnvironment.createObject("Point", {x: -104.8319, y: 39.7294, spatialReference: Factory.SpatialReference.createWgs84()});
        mapView.setViewpointCenterAndScale(viewpointCenterPoint, 2e5);
    }
}
