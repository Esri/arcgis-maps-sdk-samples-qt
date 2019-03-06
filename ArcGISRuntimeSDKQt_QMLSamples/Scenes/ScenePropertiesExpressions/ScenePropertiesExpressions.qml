// [WriteFile Name=ScenePropertiesExpressions, Category=Scenes]
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
import QtQuick.Controls 2.2

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real longitude: -118.71
    property real latitude: 32.09
    property real altitude: 100000.0
    property int coneDimension: 10000
    property real initialPitch: 90.0
    property real initialHeading: 180.0
    property string headingStr: "heading"
    property string pitchStr: "pitch"

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

        // add a graphics overlay
        GraphicsOverlay {
            id: graphicsOverlay
            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementAbsolute
            }

            SimpleRenderer {
                id: sceneRenderer
                RendererSceneProperties {
                    id: renderProps
                    headingExpression: "["+headingStr+"]"
                    pitchExpression:  "["+pitchStr+"]"
                }
            }

            Graphic {
                id: coneGraphic

                geometry: Point {
                    x: longitude
                    y: latitude
                    z: altitude
                    spatialReference: sceneView.spatialReference
                }

                SimpleMarkerSceneSymbol{
                    style: Enums.SimpleMarkerSceneSymbolStyleCone
                    color: "red"
                    width: coneDimension
                    depth: coneDimension
                    height: coneDimension * 2
                    anchorPosition: Enums.SceneSymbolAnchorPositionCenter
                }

                Component.onCompleted: {
                    coneGraphic.attributes.insertAttribute(headingStr, initialHeading);
                    coneGraphic.attributes.insertAttribute(pitchStr, initialPitch);
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCameraAndWait(camera);
        }
    }

    Rectangle {
        anchors.fill: sliderColumn
        color: "lightblue"
    }

    Column{
        id: sliderColumn

        spacing: 4

        anchors {
            left: parent.left
            top: parent.top
        }

        height: childrenRect.height

        Text {
            anchors {
                margins: 5
            }
            text: pitchStr + ": " + pitchSlider.value.toFixed(0)
            font.pixelSize: 20
            verticalAlignment: Text.AlignTop
        }

        Slider{
            id: pitchSlider
            opacity: 0.7
            height: 64

            // slider controls degrees of rotation:
            from: -90
            to: 90
            value: initialPitch
            anchors {
                margins: 5
            }

            onValueChanged: {
                coneGraphic.attributes.replaceAttribute(pitchStr, value);
            }
        }

        Text {
            anchors {
                margins: 5
            }
            text: headingStr + ": " + headingSlider.value.toFixed(0)
            verticalAlignment: Text.AlignTop
            font.pixelSize: 20
        }

        Slider{
            id: headingSlider
            opacity: 0.7
            height: 64

            // slider controls degrees of rotation:
            from: 0
            to: 360
            value: initialHeading
            anchors {
                margins: 5
            }

            onValueChanged: {
                coneGraphic.attributes.replaceAttribute(headingStr, value);
            }
        }
    }

    Camera {
        id: camera
        heading: 0.0
        pitch: 45.0
        roll: 0.0

        Point {
            x: longitude
            y: latitude - 1.0 // place the camera somewhat south of the cone
            z: altitude * 2.0 // place the camera somewhat higher than the cone
            spatialReference: SpatialReference { wkid: 4326 }
        }
    }
}
