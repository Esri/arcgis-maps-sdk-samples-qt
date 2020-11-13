// [WriteFile Name=RealisticLightingAndShadows, Category=Analysis]
// [Legal]
// Copyright 2020 Esri.

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
import Esri.ArcGISRuntime 100.10

Rectangle {
    width: 800
    height: 600

    // Create a scene view
    SceneView {
        id: sceneView
        anchors.fill: parent
        atmosphereEffect: Enums.AtmosphereEffectRealistic
        sunLighting: Enums.LightingModeLightAndShadows
        sunTime: "2018-08-10T08:30"

        Component.onCompleted: {
            setViewpointCameraAndSeconds(camera, 0)
        }

        Camera {
            id: camera
            Point {
                y: 45.54605153789073
                x: -122.69033380511073
                z: 941.0002111233771
            }
            heading: 162.58544227544266
            pitch: 60.0
            roll: 0
        }


        // create a scene, which is a default property of scene view
        Scene {
            // add a basemap
            BasemapTopographic {}

            // add a surface, which is a default property of scene
            Surface {
                // add an arcgis tiled elevation source...elevation source is a default property of surface
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            //! [add a scene service with ArcGISSceneLayer]
            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_BuildingShells/SceneServer/layers/0"
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#ffffff"
        opacity: 0.7
        Column {
            id: settingsColumn
            spacing: 5
            padding: 15
            Text {
                id: timeValue
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 16
                text: "August 10, 2018 " + sceneView.sunTime.toLocaleTimeString("hh:mm ap")
            }

            ComboBox {
                id: sunLightingSelection
                anchors.horizontalCenter: parent.horizontalCenter
                width: 200
                model: [
                    "No Light",
                    "Light",
                    "Light and Shadows"
                ]
                currentIndex: 2
            }
            Slider {
                id: sunTimeSlider
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.7
                from: 0; to: 23.99
                value: 8.5
                onValueChanged: {
                    sceneView.sunTime = doubleToDateTime(sunTimeSlider.value)
                }
            }
        }
    }
    states: [
        State {
            when: sunLightingSelection.currentIndex == 0
            PropertyChanges {
                target: sceneView
                sunLighting: Enums.LightingModeNoLight
            }
        },
        State {
            when: sunLightingSelection.currentIndex == 1
            PropertyChanges {
                target: sceneView
                sunLighting: Enums.LightingModeLight
            }
        },
        State {
            when: sunLightingSelection.currentIndex == 2
            PropertyChanges {
                target: sceneView
                sunLighting: Enums.LightingModeLightAndShadows
            }
        }
    ]

    function doubleToDateTime(d) {
        const remainder = d % 1
        let minute = remainder * 60
        let hour = d - remainder
        if (hour < 10) {
            hour = "0" + hour
        }
        if (minute < 10) {
            minute = "0" + minute
        }
        const dateTime = new Date("2018-08-10T"+hour+":"+minute)
        return dateTime
    }
}
