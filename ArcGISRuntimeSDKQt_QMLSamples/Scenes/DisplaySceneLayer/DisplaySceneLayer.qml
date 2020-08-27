// [WriteFile Name=DisplaySceneLayer, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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
    width: 800
    height: 600

    // Create a scene view
    SceneView {
        anchors.fill: parent

        // create a scene, which is a default property of scene view
        Scene {
            // add a basemap
            BasemapTopographic {}

            //! [add a scene service with ArcGISSceneLayer]
            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"
            }
            //! [add a scene service with ArcGISSceneLayer]

            // add a surface, which is a default property of scene
            Surface {
                // add an arcgis tiled elevation source...elevation source is a default property of surface
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    x: -4.49779155626782
                    y: 48.38282454039932
                    z: 62.013264927081764
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 62.013264927081764

                Camera {
                    id: camera
                    location: Point {
                        x: -4.49779155626782
                        y: 48.38282454039932
                        z: 62.013264927081764
                        spatialReference: SpatialReference { wkid: 4326 }
                    }
                    heading: 41.64729875588979
                    pitch: 71.2017391571523
                    roll: 2.194677223e-314
                }
            }
        }
    }
}
