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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    

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

            // Set an initial viewpoint
            ViewpointCenter {
                Point {
                    id: centerPt
                    x: -4.49492
                    y: 48.3808
                    z: 48.2511
                    spatialReference: Factory.SpatialReference.createWgs84()
                }
                targetScale: 62.01

                Camera {
                    id: camera
                    location: centerPt
                    heading: 344.488
                    pitch: 74.1212
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
