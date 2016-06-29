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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D/SkyCrane/SkyCrane.lwo"

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
        }

        GraphicsOverlay {
            id: graphicsOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementRelative
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCamera(camera);
            //            createSymbol();
        }
    }

    Point {
        id: point
        x: -2.708471
        y: 56.096575
        z: 5000
        spatialReference: SpatialReference.createWgs84()
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

    SimpleMarkerSymbol {
        id: sms
        style: Enums.SimpleMarkerSymbolStyleCircle
        color: "red"
        size: 10
    }

    SimpleMarkerSceneSymbol {
        id: smss
        style: Enums.SimpleMarkerSceneSymbolStyleCone
        color: "red"
        height: 75
        width: 75
        pitch: -90
    }

    ModelMarkerSymbol {
        id: mms
        url: dataPath
        scale: 0.01
        heading: 180

        Component.onCompleted: {
            mms.load();
        }

        onLoadStatusChanged: {
            if (mms.loadStatus === Enums.LoadStatusLoaded) {
                distanceCompositeSceneSymbol.distanceSymbolRanges.append(dsrModel);
                distanceCompositeSceneSymbol.distanceSymbolRanges.append(dsrCone);
                distanceCompositeSceneSymbol.distanceSymbolRanges.append(dsrCircle);

                var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                        geometry: point,
                                                                        symbol: distanceCompositeSceneSymbol
                                                                    });
                graphicsOverlay.graphics.append(graphic);
            }
        }

        DistanceSymbolRange {
            id: dsrModel
            minDistance: 0
            maxDistance: 999
            symbol: mms
        }

        DistanceSymbolRange {
            id: dsrCone
            minDistance: 1000
            maxDistance: 1999
            symbol: smss
        }

        DistanceSymbolRange {
            id: dsrCircle
            minDistance: 2000
            maxDistance: 0
            symbol: sms
        }

        DistanceCompositeSceneSymbol {
            id: distanceCompositeSceneSymbol
        }
      }
}
