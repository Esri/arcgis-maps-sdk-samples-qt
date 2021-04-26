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
import Esri.ArcGISRuntime 100.12

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

            // Add the integrated mesh layer to the scene
            IntegratedMeshLayer {
                url: "https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/Girona_Spain/SceneServer"
            }

            // Set the initial viewpoint to Yosemite Valley
            ViewpointCenter {
                center: Point {
                    id: initPt
                    x: 2.8259
                    y: 41.9906
                    z: 200.0
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 2104

                Camera {
                    location: initPt
                    heading: 190.0
                    pitch: 65.0
                    roll: 0.0
                    distance: 200.0
                }
            }
        }
    }
}
