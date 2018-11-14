// [WriteFile Name=TerrainExaggeration, Category=Scenes]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    SceneView {
        id: sceneView
        anchors.fill: parent

        // add a Scene to the SceneView
        Scene {
            // add the BasemapNationalGeographic basemap to the scene
            BasemapNationalGeographic {}

            // add a surface...surface is a default property of scene
            Surface {
                // add an arcgis tiled elevation source...elevation source is a default property of surface
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
                // elevationExaggeration is bound to the value of the slider
                elevationExaggeration: slider.value
            }
        }

        Component.onCompleted: {
            sceneView.setViewpointCamera(camera);
        }

        // Slider UI presentation at bottom
        Rectangle {
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
            }

            width: childrenRect.width
            height: childrenRect.height
            radius: 10
            opacity: 0.8

            // sliderCombo: A slider and text for its value
            Row {
                id: sliderCombo
                spacing: 5

                Slider {
                    id: slider
                    opacity: 0.7
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    // Slider controls exagerration of the elevation of
                    // the terrain
                    from: 1.0
                    to: 10.0
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        margins: 5
                    }
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 20
                    text: slider.value.toPrecision(2);
                }
            }
        }
    }

    Camera {
        id: camera
        distance: 0
        heading: 7
        pitch: 70
        roll: 0.0

        Point {
            x: -119.9616962169934
            y: 46.7000413426849
            z: 3183
            spatialReference: SpatialReference { wkid: 4326 }
        }
    }
}
