// [WriteFile Name=OrbitCameraAroundObject, Category=Scenes]
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

import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.5
import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    /* Define the Scene. Load a plane model from disk, place it above a runway, and create an orbiting camera around it */

    property string planeModelPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D/Bristol/Collada/Bristol.dae"

    SceneView {
        id: view
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

            id: sceneOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementRelative
            }

            SimpleRenderer {
                id: sceneRenderer
                RendererSceneProperties {
                    id: renderProps
                    headingExpression: "[HEADING]"
                    pitchExpression: "[PITCH]"
                }
            }

            ModelSceneSymbol {
                id: planeModel
                url: planeModelPath
            }

            Graphic {
                id: planeGraphic
                symbol: planeModel

                //Place the plane just above a runway.
                geometry: Point {
                    x: 6.637
                    y: 45.399
                    z: 100.0
                    spatialReference: SpatialReference.createWgs84()
                }

                Component.onCompleted: {
                    //The renderer is set to orient objects based on their "HEADING" and "PITCH" attributes, this is how we'll control the plane transform
                    planeGraphic.attributes.insertAttribute("HEADING", 45.0); //Line up with the runway underneath the plane
                    planeGraphic.attributes.insertAttribute("PITCH", 0.0);
                }
            }
        }

        OrbitGeoElementCameraController {
            id: orbitCam
            targetGeoElement: planeGraphic //This camera orbits the plane graphic we loaded earlier
            cameraDistance: 50.0
            minCameraHeadingOffset: -45.0
            maxCameraHeadingOffset: 45.0
            minCameraPitchOffset: 10.0
            maxCameraPitchOffset: 100.0
            maxCameraDistance: 100.0
            targetVerticalScreenFactor: 0.33 //Move the camera so the plane is more on the bottom of the screen
            autoPitchEnabled: false

            // When we move to the callback, we trigger an animation,
            // this callback handles locking the camera into its new cockpit-state when that animation is finished
            onMoveCameraStatusChanged: {
                if(moveCameraStatus === Enums.TaskStatusCompleted) {
                    minCameraPitchOffset = 90.0
                    maxCameraPitchOffset = 90.0
                    autoPitchEnabled = true; //This property locks the orbital camera pitch to the pitch of the target object
                }
            }

            // Called when the user triggers the cockpit view button (see below)
            function moveToCockpit() {
                orbitCam.cameraDistanceInteractive = false
                orbitCam.minCameraDistance = 0.0
                orbitCam.setTargetOffsets(0.0, -2.0, 1.1, 1.0)

                //The animation may rotate us over the set camera bounds based on the plane pitch, so unlock them.
                orbitCam.minCameraPitchOffset = -180.0
                orbitCam.maxCameraPitchOffset = 180.0

                //Trigger the move-into-cockpit animation.
                //If the camera is already tracking object pitch, don't want to animate the pitch any further, we're exactly where we should be.
                orbitCam.moveCamera(0.0 - orbitCam.cameraDistance,
                                    0.0 - orbitCam.cameraHeadingOffset,
                                    orbitCam.autoPitchEnabled ? 0.0 : (90.0 - orbitCam.cameraPitchOffset) + planeGraphic.attributes.attributeValue("PITCH"), 1.0)
            }

            // Called when the user triggers the center view button (see below.) Snaps back to a following view.
            function moveToFollowing() {
                orbitCam.cameraDistanceInteractive = true
                orbitCam.autoPitchEnabled = false
                orbitCam.targetOffsetX = 0.0
                orbitCam.targetOffsetY = 0.0
                orbitCam.targetOffsetZ = 0.0
                orbitCam.cameraHeadingOffset = 0.0
                orbitCam.minCameraPitchOffset = 10.0
                orbitCam.maxCameraPitchOffset = 100.0
                orbitCam.cameraPitchOffset = 45.0
                orbitCam.minCameraDistance = 10.0
                orbitCam.cameraDistance = 50.0
            }
        }

        cameraController: orbitCam //Our new orbital camera controller should be the active SceneView controller.
    }


    /* Create the UI */

    //Color for the panels that the UI controls sit on.
    property color uiBackgroundCol: Qt.rgba(0.2, 0.2, 0.2, 0.65)

    //Camera heading slider, sits at the bottom of the screen
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 30
        }

        height: cameraHeadingSlider.height + cameraHeadingLabel.height
        color: uiBackgroundCol

        Slider {
            id: cameraHeadingSlider

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            value: orbitCam.cameraHeadingOffset //Value bound to the model, so rotating the camera will update the slider.
            from: orbitCam.minCameraHeadingOffset
            to: orbitCam.maxCameraHeadingOffset

            //To avoid getting stuck in a binding loop for the value, update the value of the camera heading from the slider only in response to a moving slider.
            onMoved: {
                orbitCam.cameraHeadingOffset = value
            }

            //Custom slider handle that displays the current value
            handle: Rectangle {
                x: cameraHeadingSlider.leftPadding + cameraHeadingSlider.visualPosition * (cameraHeadingSlider.availableWidth - width)
                y: cameraHeadingSlider.topPadding + cameraHeadingSlider.availableHeight / 2 - height / 2
                implicitWidth: 30
                implicitHeight: 30

                Text {
                    id: headingValue

                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }

                    text: Math.round(cameraHeadingSlider.value)
                    color: "black"
                }
            }
        }

        Text {
            id: cameraHeadingLabel

            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: cameraHeadingSlider.top
            }

            text: "Camera Heading"
            color: "white"
        }
    }

    //Plane pitch slider, placed in the top-right of the screen
    Rectangle {
        anchors {
            top: parent.top
            bottom: parent.verticalCenter
            right: parent.right
            margins: 30
        }

        width: childrenRect.width
        color: uiBackgroundCol

        ColumnLayout
        {
            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            spacing: 5

            Text {
                id: planePitchLabel
                text: "Plane Pitch"
                color: "white"

            }

            Slider {
                id: planePitchSlider

                Layout.fillHeight: true

                value: 0
                from: 90
                to: -90
                orientation: Qt.Vertical

                onMoved: {
                    planeGraphic.attributes.replaceAttribute("PITCH", value)
                }

                //Custom slider handle that displays the current value
                handle: Rectangle {
                    x: planePitchSlider.leftPadding + planePitchSlider.availableWidth / 2 - width / 2
                    y: planePitchSlider.topPadding + planePitchSlider.visualPosition * (planePitchSlider.availableHeight - height)

                    implicitWidth: 30
                    implicitHeight: 30

                    Text {
                        id: pitchValue

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                        }

                        text: Math.round(planePitchSlider.value)
                        color: "black"
                    }
                }
            }
        }
    }

    //View change buttons / allow cam interaction checkbox placed in the top-left of the screen.
    Rectangle {
        anchors {
            left: parent.left
            top: parent.top
        }

        height: childrenRect.height
        width: childrenRect.width + 30 //Add a little bit of manual padding to comfortably fit the checkbox text
        color: uiBackgroundCol

        Column {
            spacing: 10
            padding: 10
            Button {
                text: "Cockpit View"
                onClicked: orbitCam.moveToCockpit()
            }

            Button {
                text: "Center View"
                onClicked: orbitCam.moveToFollowing()
            }

            Row {
                anchors {
                    left: parent.left
                }

                CheckBox {
                    id: allowCamDistanceInteractionCheckBox
                    checked: orbitCam.cameraDistanceInteractive
                    onCheckedChanged: orbitCam.cameraDistanceInteractive = checked
                }

                Text {
                    anchors {
                        verticalCenter: allowCamDistanceInteractionCheckBox.verticalCenter
                    }
                    text: "Allow camera\ndistance interaction"
                    color: "white"
                }
            }
        }
    }
}
