// [WriteFile Name=FeatureLayerExtrusion, Category=Scenes]
// [Legal]
// Copyright 2017 Esri.

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
import Esri.ArcGISRuntime 100.4

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        Button {
            anchors{
                bottom: sceneView.attributionTop
                horizontalCenter: sceneView.horizontalCenter
                bottomMargin: 10
            }
            id: popButton
            text: "TOTAL POPULATION"
            onClicked: {
                if (text === "TOTAL POPULATION") {
                    text = qsTr("POPULATION DENSITY");
                    sceneProperties.extrusionExpression = "([POP07_SQMI] * 5000) + 10000";
                } else {
                    text = qsTr("TOTAL POPULATION");
                    sceneProperties.extrusionExpression = "[POP2007] / 10";
                }
            }
        }

        Scene {
            id: scene
            BasemapImagery {}

            ServiceFeatureTable {
                id: featureTable
                url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer/3"
            }

            FeatureLayer {
                id: featureLayer
                featureTable: featureTable
                renderingMode: Enums.FeatureRenderingModeDynamic
                renderer: renderer
            }

            Surface {
                id: baseSurface
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            ViewpointCenter {
                Point {
                    id: lookAtPoint
                    x: -99.659448
                    y: 20.513652
                    z: 12940924
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 12940924

                Camera {
                    id: initialCamera
                    location: lookAtPoint
                    roll: 0
                    pitch: 15
                    heading: 0
                }
            }
        }

        SimpleRenderer {
            id: renderer
            symbol: fillSymbol
            sceneProperties: sceneProperties
        }

        RendererSceneProperties {
            id: sceneProperties
            extrusionMode: Enums.ExtrusionModeAbsoluteHeight
            extrusionExpression: "[POP2007] / 10"
        }

        SimpleFillSymbol {
            id: fillSymbol
            color: "blue"
            outline: lineSymbol
        }

        SimpleLineSymbol {
            id: lineSymbol
            color: "black"
        }
    }
}
