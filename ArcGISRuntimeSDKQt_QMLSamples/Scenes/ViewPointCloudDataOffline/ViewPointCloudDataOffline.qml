// [WriteFile Name=ViewPointCloudDataOffline, Category=Scenes]
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
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/slpk/"

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

            // Add a point cloud layer to the scene
            PointCloudLayer {

                // set the URL to a local scene layer package
                url: dataPath + "sandiego-north-balboa-pointcloud.slpk"

                // zoom to the layer once loaded
                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    var vp = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: fullExtent});
                    sceneView.setViewpoint(vp);
                }
            }
        }
    }
}
