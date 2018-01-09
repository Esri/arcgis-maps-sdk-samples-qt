// [WriteFile Name=DistanceCompositeSymbol, Category=Scenes]
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
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"

    // Create a scene view
    SceneView {
        id: sceneView
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

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    // create a graphic using the composite symbol
                    var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                            geometry: point,
                                                                            symbol: distanceCompositeSceneSymbol
                                                                        });
                    // add the graphic to the graphics overlay
                    graphicsOverlay.graphics.append(graphic);
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCameraAndWait(camera)
        }

        GraphicsOverlay {
            id: graphicsOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementRelative
            }
        }
    }

    Point {
        id: point
        x: -2.708471
        y: 56.096575
        z: 5000
        spatialReference: SpatialReference { wkid: 4326 }
    }

    // create the camera to be used as the scene view's viewpoint
    Camera {
        id: camera
        location: point
        distance: 1500
        heading: 0
        pitch: 80.0
        roll: 0
    }    

    //! [create a distance composite scene symbol]
    DistanceCompositeSceneSymbol {
        id: distanceCompositeSceneSymbol

        // create a distance symbol range with a model scene symbol
        DistanceSymbolRange {
            minDistance: 0
            maxDistance: 999

            //! [model scene symbol]
            ModelSceneSymbol {
                id: mms
                url: dataPath + "3D/SkyCrane/SkyCrane.lwo"
                scale: 0.01
                heading: 180
            }
            //! [model scene symbol]
        }

        // create a distance symbol range with a simple marker scene symbol
        DistanceSymbolRange {
            minDistance: 1000
            maxDistance: 1999

            //! [simple marker scene symbol]
            SimpleMarkerSceneSymbol {
                style: Enums.SimpleMarkerSceneSymbolStyleCone
                color: "red"
                height: 75
                width: 75
                depth: 75
            }
            //! [simple marker scene symbol]
        }

        // create a distance symbol range with a simple marker symbol
        DistanceSymbolRange {
            minDistance: 2000
            maxDistance: 0

            SimpleMarkerSymbol {
                style: Enums.SimpleMarkerSymbolStyleCircle
                color: "red"
                size: 10
            }
        }
    }
    //! [create a distance composite scene symbol]
}

