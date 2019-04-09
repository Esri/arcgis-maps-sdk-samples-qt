// [WriteFile Name=FeatureLayerRenderingModeScene, Category=Layers]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool zoomedOut: true    

    SceneView {
        id: topSceneView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2

        Scene {
            // Set the preferred rendering mode to static
            LoadSettings {
                preferredPointFeatureRenderingMode: Enums.FeatureRenderingModeStatic
                preferredPolygonFeatureRenderingMode: Enums.FeatureRenderingModeStatic
                preferredPolylineFeatureRenderingMode: Enums.FeatureRenderingModeStatic
            }

            // Add the Feature Layers
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/0"
                }
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/8"
                }
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;
                topSceneView.setViewpointCameraAndWait(zoomOutCamera);
            }
        }

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                margins: 5
            }
            color: "white"
            radius: 5
            width: 200
            height: 30

            Text {
                anchors.centerIn: parent
                text: "Rendering Mode Static"
            }
        }
    }

    SceneView {
        id: bottomSceneView
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 2

        Scene {
            // Set the preferred rendering mode to dynamic
            LoadSettings {
                preferredPointFeatureRenderingMode: Enums.FeatureRenderingModeDynamic
                preferredPolygonFeatureRenderingMode: Enums.FeatureRenderingModeDynamic
                preferredPolylineFeatureRenderingMode: Enums.FeatureRenderingModeDynamic
            }

            // Add the Feature Layers
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/0"
                }
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/8"
                }
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Energy/Geology/FeatureServer/9"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;
                bottomSceneView.setViewpointCameraAndWait(zoomOutCamera);
            }
        }

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                margins: 5
            }
            color: "white"
            radius: 5
            width: 200
            height: 30

            Text {
                anchors.centerIn: parent
                text: "Rendering Mode Dynamic"
            }
        }
    }

    Camera {
        id: zoomOutCamera

        Point {
            x: -118.37
            y: 34.46
            spatialReference: SpatialReference { wkid: 4326 }
        }

        distance: 42000
        heading: 0
        pitch: 0
        roll: 0
    }

    Camera {
        id: zoomInCamera

        Point {
            x: -118.45
            y: 34.395
            spatialReference: SpatialReference { wkid: 4326 }
        }

        distance: 2500
        heading: 90
        pitch: 75
        roll: 0
    }

    Button {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        property string startText: "Start Animation"
        property string stopText: "Stop Animation"
        text: startText
        onClicked: {
            if (text === startText) {
                timer.start();
                text = stopText;
            } else {
                timer.stop();
                text = startText;
            }
        }
    }

    Timer {
        id: timer
        interval: 7000
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            var camera;
            if (zoomedOut)
                camera = zoomInCamera;
            else
                camera = zoomOutCamera;
            topSceneView.setViewpointCameraAndSeconds(camera, 5);
            bottomSceneView.setViewpointCameraAndSeconds(camera, 5);
            zoomedOut = !zoomedOut;
        }
    }
}
