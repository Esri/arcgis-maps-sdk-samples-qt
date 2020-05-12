// [WriteFile Name=ExploreScenesInFlyoverAR, Category=AR]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1
import Esri.ArcGISArToolkit 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    ArcGISArView {
        id: arcGISArView
        anchors.fill: parent
        tracking: true
        sceneView: sceneView
    }

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                opacity: 0.0
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // create the integrated mesh layer
            IntegratedMeshLayer {
                id: integratedMeshLayer
                url: "https://www.arcgis.com/home/item.html?id=dbc72b3ebb024c848d89a42fe6387a1b"

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    // Enable subsurface navigation. This allows you to look at the scene from below.
                    sceneView.scene.baseSurface.navigationConstraint = Enums.NavigationConstraintStayAbove;
                    scene.baseSurface.opacity = 1.0;

                    // Start with the camera at the center of the mesh layer.
                    const viewPoint = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                                y: integratedMeshLayer.fullExtent.center.y,
                                                                                x: integratedMeshLayer.fullExtent.center.x,
                                                                                z: 250.0
                                                                            });

                    const originCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                                   location: viewPoint,
                                                                                   heading: 0,
                                                                                   pitch: 90,
                                                                                   roll: 0
                                                                               });

                    arcGISArView.originCamera = originCamera;

                    // Set the translation factor to enable rapid movement through the scene.
                    arcGISArView.translationFactor = 1000;

                    // Enable atmosphere and space effects for a more immersive experience.
                    sceneView.spaceEffect = Enums.SpaceEffectStars;
                    sceneView.atmosphereEffect = Enums.AtmosphereEffectRealistic;
                }

                onErrorChanged: {
                    console.log("(%1)[%2] %3 - %4".arg(error.code).arg(error.domain).arg(error.message).arg(error.additionalMessage));
                }
            }
        }
    }
}
