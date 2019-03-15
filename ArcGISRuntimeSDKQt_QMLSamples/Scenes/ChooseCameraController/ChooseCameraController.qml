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
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1
import QtQuick.Controls 2.2

Rectangle {
    clip: true
    width: 800
    height: 600

    property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data/3D"

    property real longitude: -109.929589
    property real latitude: 38.43500
    property real altitude: 10000
    property real cameraDist: 5000

    GlobeCameraController {
        id: globeController
    }

    Point {
        id: theCrater
        x: longitude
        y: latitude
        z: altitude
        spatialReference: SpatialReference { wkid: 4326 }
    }

    OrbitLocationCameraController {
        id: orbitLocationCtrlr
        targetLocation: theCrater
        cameraDistance: cameraDist
    }

    OrbitGeoElementCameraController{
        id: orbitPlaneCtrlr
        targetGeoElement: thePlane
        cameraDistance: cameraDist / 2
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
                    z: altitude / 4
                    spatialReference: sceneView.spatialReference
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCameraAndWait(camera);
        }
    }

    Camera {
        id: camera
        heading: 0.0
        pitch: 0.0
        roll: 0.0

        Point {
            x: longitude
            y: latitude
            z: altitude
            spatialReference: SpatialReference { wkid: 4326 }
        }
    }

    // Create a rectangle to display the GUI
    Rectangle {
        id: layerVisibilityRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 150
        width: 250
        color: "transparent"

        Rectangle {
            anchors.fill: parent
            width: layerVisibilityRect.width
            height: layerVisibilityRect.height
            color: "lightgrey"
            opacity: .8
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10
                }

                Text {
                    width: parent.width
                    text: "Choose Camera Control"
                    clip: true
                    font {
                        pointSize: 14
                        bold: true
                    }
                }

                RadioButton {
                    id: orbitAroundAeroplane
                    text: "Orbit Camera Around Plane"
                    onCheckedChanged: {
                        if (checked) {
                            sceneView.cameraController = orbitPlaneCtrlr;
                        }
                    }
                }

                RadioButton {
                    id: orbitLocation
                    text: "Orbit Camera Around Crater"
                    onCheckedChanged: {
                        if (checked) {
                            sceneView.cameraController = orbitLocationCtrlr;
                        }
                    }
                }

                RadioButton {
                    id: orbitAroundGlobe
                    checked: true
                    text: "Free Pan Round the Globe"
                    onCheckedChanged: {
                        if (checked) {
                            sceneView.cameraController = globeController;
                        }
                    }
                }
            }
        }
    }
}
