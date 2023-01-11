// [WriteFile Name=DisplayFeatureLayers, Category=Layers]
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

            // This sample initializes with the service feature table feature layer visible
            initialViewpoint: ViewpointCenter {
                id: serviceFeatureTableViewpoint
                center: Point {
                    x: -13176752
                    y: 4090404
                    spatialReference: SpatialReference { wkid: 102100 }
                }
                targetScale: 300000
            }

            // Display a feature layer with a Geodatabase
            FeatureLayer {
                id: gdbFeatureLayer
                visible: false

                featureTable: geodatabase.geodatabaseFeatureTablesByTableName["Trailheads"] ?? null
                Geodatabase {
                    id: geodatabase
                    path: dataPath + "geodatabase/LA_Trails.geodatabase"
                }
            }

            // Display a GeoPackage feature layer
            FeatureLayer {
                id: gpkgFeatureLayer
                visible: false

                GeoPackage {
                    id: gpkg
                    path: dataPath + "gpkg/AuroraCO.gpkg"

                    // Wait for the GeoPackage to load successfully
                    onLoadStatusChanged: {
                        if (loadStatus !== Enums.LoadStatusLoaded || // the GeoPackage hasn't yet loaded
                                gpkg.geoPackageFeatureTables.length === 0) // the GeoPackage has no feature tables to use
                            return;

                        gpkgFeatureLayer.featureTable = gpkg.geoPackageFeatureTables[0];
                    }

                    Component.onCompleted: {
                        load();
                    }
                    // Note you must call close() on GeoPackage to allow other processes to access it
                }
            }

            // Display a feature layer with a portal item
            FeatureLayer {
                id: portalItemFeatureLayer
                visible: false
                item: PortalItem {
                    itemId: "1759fd3e8a324358a0c58d9a687a8578"
                }
            }

            // Display a service feature table feature layer
            FeatureLayer {
                id: serviceFeatureTableFeatureLayer
                featureTable: ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"
                }
            }

            // Display a shapefile feature layer
            FeatureLayer {
                id: shapefileFeatureLayer
                visible: false
                featureTable: ShapefileFeatureTable {
                    path: dataPath + "shp/ScottishWildlifeTrust_ReserveBoundaries_20201102.shp"
                }
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
        width: 225
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
                currentIndex: 3
                onCurrentTextChanged: {
                    map.operationalLayers.forEach(layer => {layer.visible = false;});

                    switch (currentText) {
                    case "Geodatabase":
                        gdbFeatureLayer.visible = true;
                        mapView.setViewpointGeometry(gdbFeatureLayer.fullExtent);
                        break;
                    case "GeoPackage":
                        gpkgFeatureLayer.visible = true;
                        mapView.setViewpointGeometry(gpkgFeatureLayer.fullExtent);
                        break;
                    case "Portal Item":
                        portalItemFeatureLayer.visible = true;
                        mapView.setViewpointGeometryAndPadding(portalItemFeatureLayer.fullExtent, -1e4);
                        break;
                    case "Service Feature Table":
                        serviceFeatureTableFeatureLayer.visible = true;
                        mapView.setViewpoint(serviceFeatureTableViewpoint);
                        break;
                    case "Shapefile":
                        shapefileFeatureLayer.visible = true;
                        mapView.setViewpointGeometry(shapefileFeatureLayer.fullExtent);
                        break;
                    default:
                        break;
                    }

                    // Return focus to the MapView for keyboard navigation
                    mapView.forceActiveFocus();
                }
            }
        }
    }
}
