// [WriteFile Name=FilterFeaturesInScene, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.

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
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }

        Scene {
            Basemap {
                ArcGISVectorTiledLayer {
                    item: PortalItem {itemId: "1e7d1784d1ef4b79ba6764d0bd6c3150"}
                }
                ArcGISSceneLayer {
                    id: osmBuildingsSceneLayer
                    item: PortalItem {itemId: "ca0470dbbddb4db28bad74ed39949e25"}

                    polygonFilter: osmSceneLayerPolygonFilter {
                        id: osmsceneLayerPolygonFilter
                        polygons: []
                        spatialRelationship: Enums.osmSceneLayerPolygonFilterSpatialRelationshipDisjoint

                        onPolygonsChanged: {
                            console.log("polygons changed", polygons);
                        }
                    }
                }
            }

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            ArcGISSceneLayer {
                id: detailedBuildingsSceneLayer
                url: "https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/SanFrancisco_Bldgs/SceneServer"

                property Polygon extentPolygon: null

                // Initially set visibility to false for sample demonstration
                visible: false

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    const extent = fullExtent;

                    // Create a polygon builder
                    const polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {
                                                                                     spatialReference: Factory.SpatialReference.createWgs84()
                                                                                 });

                    polygonBuilder.addPointXY(extent.xMin, extent.yMin);
                    polygonBuilder.addPointXY(extent.xMax, extent.yMin);
                    polygonBuilder.addPointXY(extent.xMax, extent.yMax);
                    polygonBuilder.addPointXY(extent.xMin, extent.yMax);

                    // Save the extent polygon to later apply it to the osmSceneLayerPolygonFilter when the user clicks the button
                    extentPolygon = polygonBuilder.geometry;

                    // Create a red border to show the extent of the detailed buildings layer
                    const extentFillSymbol = ArcGISRuntimeEnvironment
                    .createObject("SimpleFillSymbol", {
                                      style: Enums.SimpleFillSymbolStyleNull,
                                      color: "transparent",
                                      outline: ArcGISRuntimeEnvironment
                                      .createObject("SimpleLineSymbol", {
                                                        style: Enums.SimpleLineSymbolStyleSolid,
                                                        color: "red",
                                                        width: 5
                                                    })
                                  });

                    const extentGraphic = ArcGISRuntimeEnvironment
                    .createObject("Graphic", {
                                      geometry: extentPolygon,
                                      symbol: extentFillSymbol
                                  });

                    graphicsOverlay.graphics.append(extentGraphic);
                }
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    id: initialPoint
                    x: -122.421
                    y: 37.7041
                    z: 207
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 1000

                Camera {
                    id: camera
                    location: initialPoint
                    heading: 60
                    pitch: 70
                    roll: 0
                }
            }
        }

        GraphicsOverlay {
            id: graphicsOverlay

            // Initially set visibility to false for sample demonstration
            visible: false
        }
    }

    Button {
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }

        property int step: 0

        text: ["Load detailed buildings","Filter OSM buildings in extent","Reset scene"][step]
        onClicked: {
            switch (step) {
            case 0:
                // Show the detailed buildings scene layer and extent graphic
                detailedBuildingsSceneLayer.visible = true;
                graphicsOverlay.visible = true;
                step++;
                break;

            case 1:
                // Set the osmSceneLayerPolygonFilter's polygons to the extent of the detailed buildings layer
                // to hide all OSM buildings within that area
                osmsceneLayerPolygonFilter.polygons = [detailedBuildingsSceneLayer.extentPolygon];
                step++;
                break;

            case 2:
                // Reset the scene to its original state
                detailedBuildingsSceneLayer.visible = false;
                graphicsOverlay.visible = false;
                osmsceneLayerPolygonFilter.polygons = [];
                step = 0;
                break;

            default:
                step = 0;
                break;
            }
        }
    }
}
