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
import QtQuick.Controls 2.2
import Esri.Samples 1.0

Item {

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ChooseCameraControllerSample {
        id: chooseCameraControllerSample
        sceneView: view
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
                            chooseCameraControllerSample.setOrbitGeoElementController();
                        }
                    }
                }

                RadioButton {
                    id: orbitLocation
                    text: "Orbit Camera Around Crater"
                    onCheckedChanged: {
                        if (checked) {
                            chooseCameraControllerSample.setOrbitLocationController();
                        }
                    }
                }

                RadioButton {
                    id: orbitAroundGlobe
                    checked: true
                    text: "Free Pan Round the Globe"
                    onCheckedChanged: {
                        if (checked) {
                            chooseCameraControllerSample.setGlobeController();
                        }
                    }
                }
            }
        }
    }
}
