// [WriteFile Name=ChooseCameraController, Category=Scenes]
// [Legal]
// Copyright 2019 Esri.

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
import Esri.ArcGISExtras 1.1
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

Rectangle {
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data/3D"
    readonly property real longitude: -109.929589
    readonly property real latitude: 38.43500
    readonly property real distance: 5000

    GlobeCameraController {
        id: globeController
    }

    Point {
        id: theCrater
        x: longitude
        y: latitude
        z: distance
        spatialReference: SpatialReference { wkid: 4326 }
    }

    OrbitLocationCameraController {
        id: orbitLocationCtrlr
        targetLocation: theCrater
        cameraDistance: distance
    }

    OrbitGeoElementCameraController{
        id: orbitPlaneCtrlr
        targetGeoElement: thePlane
        cameraDistance: distance / 2
    }

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
        }

        GraphicsOverlay {
            id: graphicsOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementRelative
            }

            ModelSceneSymbol {
                id: mms
                url: dataPath + "/Bristol/Collada/Bristol.dae"
                scale: 10.0
                heading: 0.0
            }

            Graphic {
                id: thePlane
                symbol: mms

                geometry: Point {
                    x: longitude
                    y: latitude
                    z: distance / 2
                    spatialReference: sceneView.spatialReference
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            sceneView.setViewpointCamera(camera);
        }
    }

    Camera {
        id: camera
        heading: 0.0
        pitch: 0.0
        roll: 0.0
        location: theCrater
    }

    // Create a rectangle to display the GUI
    Rectangle {
        width: childrenRect.width
        height: childrenRect.height
        color: "lightgrey"
        opacity: 0.8
        radius: 5
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        border {
            color: "#4D4D4D"
            width: 1
        }

        ColumnLayout {

            Text {
                Layout.margins: 5
                Layout.fillWidth: true
                text: "Choose camera controller"
                clip: true
                font {
                    pointSize: 14
                    bold: true
                }
            }

            RadioButton {
                id: orbitAroundAeroplane
                Layout.fillWidth: true
                text: "Orbit camera around plane"
                onCheckedChanged: {
                    if (checked) {
                        sceneView.cameraController = orbitPlaneCtrlr;
                    }
                }
            }

            RadioButton {
                id: orbitLocation
                Layout.fillWidth: true
                text: "Orbit camera around crater"
                onCheckedChanged: {
                    if (checked) {
                        sceneView.cameraController = orbitLocationCtrlr;
                    }
                }
            }

            RadioButton {
                id: orbitAroundGlobe
                Layout.fillWidth: true
                checked: true
                text: "Free pan round the globe"
                onCheckedChanged: {
                    if (checked) {
                        sceneView.cameraController = globeController;
                    }
                }
            }
        }
    }
}
