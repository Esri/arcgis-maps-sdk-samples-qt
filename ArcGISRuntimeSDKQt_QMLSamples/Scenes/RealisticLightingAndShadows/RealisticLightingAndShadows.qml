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
        color: "#eeeeee"
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
                width: sceneView.width *.25
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
                width: sceneView.width *.33
                opacity: 0.7
                from: 0; to: 23.99
                value: 8.5
                onValueChanged: {
                    sceneView.sunTime = doubleToDateTime(sunTimeSlider.value)
                }
                background: Rectangle {
                    id: bar
                    color: "black"
                    height: sunTimeSlider.height*.1
                    anchors.centerIn: sunTimeSlider
                    width: parent.width *.85
                }
                Text {
                    text: "|"
                    x: bar.x
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text: "|"
                    x: bar.x + bar.width * .24
                    anchors.verticalCenter: parent.verticalCenter                }
                Text {
                    text: "|"
                    x: bar.x + bar.width * .49
                    anchors.verticalCenter: parent.verticalCenter                }
                Text {
                    text: "|"
                    x: bar.x + bar.width * .74
                    anchors.verticalCenter: parent.verticalCenter                }
                Text {
                    text: "|"
                    x: bar.x + bar.width * .99
                    anchors.verticalCenter: parent.verticalCenter                }
                Text {
                    text: "0"
                    x: parent.x
                    anchors.bottom: sunTimeSlider.bottom
                }
                Text {
                    text: "6"
                    x: parent.x + parent.width * .22
                    anchors.bottom: sunTimeSlider.bottom
                }
                Text {
                    text: "12"
                    x: parent.x + parent.width * .42
                    anchors.bottom: sunTimeSlider.bottom
                }
                Text {
                    text: "18"
                    x: parent.x + parent.width * .62
                    anchors.bottom: sunTimeSlider.bottom
                }
                Text {
                    text: "24"
                    x: parent.x + parent.width*.82
                    anchors.bottom: sunTimeSlider.bottom
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
        //return "2018-08-10T"+hour+":00"
    }
}
