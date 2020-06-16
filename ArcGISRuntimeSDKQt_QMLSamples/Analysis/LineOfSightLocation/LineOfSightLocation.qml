// [WriteFile Name=LineOfSightLocation, Category=Analysis]
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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.9

Rectangle {
    clip: true
    width: 800
    height: 600

    
    property bool calculating: false

    SceneView {
        id: sceneView
        anchors.fill: parent
        focus: true

        Scene {
            id: scene
            BasemapTopographic {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    setInitialViewpoint();
                }
            }
        }

        // Add an Analysis Overlay
        AnalysisOverlay {
            id: analysisOverlay

            // Declare a LineOfSight and set initial observer and target locations
            LocationLineOfSight {
                id: locationLineOfSight
                visible: true
                observerLocation: Point {
                    x: -73.06958032962375
                    y: -49.253112971555446
                    z: 2000
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
                targetLocation: Point {
                    x: -73.079266999709162
                    y: -49.300457676730559
                    z: 1312
                    spatialReference: Factory.SpatialReference.createWgs84()
                }

                // Configure the LoS color and width
                Component.onCompleted: {
                    LineOfSight.setVisibleColor("cyan");
                    LineOfSight.setObstructedColor("magenta");
                    LineOfSight.setLineWidth(2);
                }
            }
        }

        // Update the target location as you tap the map
        onMouseClicked: locationLineOfSight.targetLocation = sceneView.screenToBaseSurface(mouse.x, mouse.y);
        onMousePressedAndHeld: calculating = true;
        onMouseReleased: calculating = false;
        onMousePositionChanged: {
            if (!calculating)
                return;

            locationLineOfSight.targetLocation = sceneView.screenToBaseSurface(mouse.x, mouse.y);
        }
    }

    function setInitialViewpoint() {
        // Set a viewpoint
        const point = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                x: -73.0815,
                                                                y: -49.3272,
                                                                z: 4059,
                                                                spatialReference: Factory.SpatialReference.createWgs84()
                                                            });

        const camera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                 location: point,
                                                                 heading: 11,
                                                                 pitch: 62,
                                                                 roll: 0
                                                             });

        sceneView.setViewpointCamera(camera);
    }
}
