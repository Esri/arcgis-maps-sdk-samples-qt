// [WriteFile Name=AddIntegratedMeshLayer, Category=Scenes]
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

            Surface {
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // Add the integrated mesh layer to the scene
            IntegratedMeshLayer {
                url: "https://tiles.arcgis.com/tiles/FQD0rKU8X5sAQfh8/arcgis/rest/services/VRICON_Yosemite_Sample_Integrated_Mesh_scene_layer/SceneServer"
            }

            // Set the initial viewpoint to Yosemite Valley
            ViewpointCenter {
                center: Point {
                    id: initPt
                    x: -119.622075
                    y: 37.720650
                    z: 2104.901239
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 2104

                Camera {
                    location: initPt
                    heading: 315.50368761552056
                    pitch: 78.09465920130114
                    roll: 0
                    distance: 2104
                }
            }
        }
    }
}
