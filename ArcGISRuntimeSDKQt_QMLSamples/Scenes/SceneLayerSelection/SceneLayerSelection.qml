// [WriteFile Name=SceneLayerSelection, Category=Scenes]
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

            // add a scene layer
            ArcGISSceneLayer {
                id: sceneLayer
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    id: initialPoint
                    x: -4.49779155626782
                    y: 48.38282454039932
                    z: 62.013264927081764
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 62.013264927081764

                Camera {
                    id: camera
                    location: initialPoint
                    heading: 41.64729875588979
                    pitch: 71.2017391571523
                    roll: 2.194677223e-314
                }
            }

            // add an elevation surface
            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }
        }

        // handle the identifyLayerStatusChanged signal
        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus !== Enums.TaskStatusCompleted)
                return;

            // get the results
            const result = sceneView.identifyLayerResult;
            const geoElements = result.geoElements;

            // get the first GeoElement
            if (geoElements.length > 0) {
                // select the feature
                sceneLayer.selectFeature(geoElements[0]);
            }
        }

        // when the scene is clicked, identify the clicked feature and select it
        onMouseClicked: {
            // clear any previous selection
            sceneLayer.clearSelection();

            // identify from the click
            sceneView.identifyLayer(sceneLayer, mouse.x, mouse.y, 10, false);
        }
    }
}
