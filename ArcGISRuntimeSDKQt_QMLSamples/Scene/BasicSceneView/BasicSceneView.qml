// [WriteFile Name=GORenderers, Category=DisplayInformation]
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Create a scene view
    SceneView {
        anchors.fill: parent

        // create a scene...scene is a default property of sceneview
        // and thus will get added to the sceneview
        Scene {
            // add a basemap
            BasemapImagery {}

            // add a surface...surface is a default property of scene
            Surface {
                // add an arcgis tiled elevation source...elevation source is a default property of surface
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // set the viewpoint extent...viewpoint extent is a default property ofthe scene
            ViewpointExtent{
                Envelope {
                    xMin: 83.70
                    yMin: 28.69
                    xMax: 84.31
                    yMax: 29.15
                    spatialReference: SpatialReference.createWgs84()
                }

                // create a camera object..camera is a default property of viewpoint extent
                Camera {
                    id: camera
                    location: Point {
                        x: 83.9
                        y: 28.4
                        z: 10010.0
                        spatialReference: SpatialReference.createWgs84()
                    }
                    pitch: 80.0
                    roll: 300.0
                    heading: 10.0
                }
            }
        }
    }
}
