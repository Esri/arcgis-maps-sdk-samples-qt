// [WriteFile Name=ViewshedGeoElement, Category=Analysis]
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

    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"
    readonly property string headingAttr: "HEADING"
    readonly property var linearUnit: Enums.LinearUnitIdMeters
    readonly property var angularUnit: Enums.AngularUnitIdDegrees
    readonly property var geodeticCurveType: Enums.GeodeticCurveTypeGeodesic
    property Point waypoint: null

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            // Set the Scene's Surface
            Surface {
                ArcGISTiledElevationSource {
                    url: "https://scene.arcgis.com/arcgis/rest/services/BREST_DTM_1M/ImageServer"
                }
            }

            // Add a Scene Layer
            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                // Set the Camera Controller
                const controller = ArcGISRuntimeEnvironment.createObject("OrbitGeoElementCameraController", {
                                                                             targetGeoElement: tank, cameraDistance: 200,
                                                                             cameraPitchOffset: 45
                                                                         });
                sceneView.cameraController = controller;
            }
        }

        // Declare a GraphicsOverlay
        GraphicsOverlay {

            SimpleRenderer {
                // set up heading expression for the tank
                RendererSceneProperties {
                    headingExpression: "[%1]".arg(headingAttr)
                }
            }

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementRelative
            }

            // Create the Tank Graphic
            Graphic {
                id: tank

                Point {
                    x: -4.508708007847015
                    y: 48.38823243446344
                    z: 0
                    spatialReference: Factory.SpatialReference.createWgs84()
                }

                ModelSceneSymbol {
                    id: sceneSymbol
                    url: dataPath + "/3D/bradley_low_3ds/bradle.3ds"
                    anchorPosition: Enums.SceneSymbolAnchorPositionBottom
                    scale: 10
                    heading: 90.0
                }

                Component.onCompleted: {
                    tank.attributes.insertAttribute(headingAttr, 150.0)
                }
            }
        }

        // Declare an Analysis Overlay
        AnalysisOverlay {
            id: analysisOverlay

            // Create the Location Viewshed
            GeoElementViewshed {
                id: geoelementViewshed
                horizontalAngle: 90.0
                verticalAngle: 25.0
                minDistance: 5.0
                maxDistance: 250.0
                headingOffset: 0
                pitchOffset: 0
                offsetY: 0.5
                offsetZ: 0.5
                geoElement: tank
            }
        }

        onMouseClicked: {
            waypoint = sceneView.screenToBaseSurface(mouse.x, mouse.y);
            timer.start();
        }
    }

    function animate() {
        if (waypoint === null)
            return;

        // get current location and distance from waypoint
        let location = tank.geometry;
        const distance = GeometryEngine.distanceGeodetic(location, waypoint, linearUnit, angularUnit, geodeticCurveType);

        // move toward waypoint based on speed and update orientation
        location = GeometryEngine.moveGeodetic([location], 1.0, linearUnit, distance.azimuth1, angularUnit, geodeticCurveType);
        tank.geometry = location[0];

        // update the heading
        const heading = tank.attributes.attributeValue(headingAttr);
        tank.attributes.replaceAttribute(headingAttr, heading + (distance.azimuth1 - heading) / 10);

        // reached waypoint
        if (distance.distance <= 5) {
            waypoint = null;
            timer.stop();
        }
    }

    Timer {
        id: timer
        interval: 100
        repeat: true
        onTriggered: animate()
    }
}
