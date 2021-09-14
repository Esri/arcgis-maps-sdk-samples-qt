// [WriteFile Name=ViewshedCamera, Category=Analysis]
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
import Esri.ArcGISRuntime 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        // Set and keep the focus on SceneView to enable keyboard navigation
        onFocusChanged: focus = true;
        Component.onCompleted: {
            forceActiveFocus();
        }

        Scene {
            id: scene
            Basemap {
                initStyle: Enums.BasemapStyleArcGISImageryStandard
            }

            // Set the Scene's Surface
            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // Add the Girona, Spain integrated mesh layer
            IntegratedMeshLayer {
                url: "https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer"
            }

            // Set an initial viewpoint
            ViewpointCenter {
                Point {
                    id: centerPt
                    x: 2.82691
                    y: 41.98500
                    z: 124.987
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
                targetScale: 1

                Camera {
                    id: camera
                    location: centerPt
                    heading: 332.131
                    pitch: 82.4732
                    roll: 0
                }
            }
        }

        // Declare an Analysis Overlay
        AnalysisOverlay {
            // Create the Location Viewshed
            LocationViewshed {
                id: locationViewshed
                minDistance: 1
                maxDistance: 1000
                frustumOutlineVisible: true
            }
        }
    }

    // calculate the viewshed on clicked
    Button {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        text: "Calculate Viewshed"
        onClicked: locationViewshed.updateFromCamera(sceneView.currentViewpointCamera);
    }
}
