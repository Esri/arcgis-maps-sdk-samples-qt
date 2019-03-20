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
import QtQuick.Layouts 1.1

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
                Layout.fillWidth: true
                id: orbitAroundAeroplane
                text: "Orbit camera around plane"
                onCheckedChanged: {
                    if (checked) {
                        chooseCameraControllerSample.setOrbitGeoElementController();
                    }
                }
            }

            RadioButton {
                Layout.fillWidth: true
                id: orbitLocation
                text: "Orbit camera around crater"
                onCheckedChanged: {
                    if (checked) {
                        chooseCameraControllerSample.setOrbitLocationController();
                    }
                }
            }

            RadioButton {
                Layout.fillWidth: true
                id: orbitAroundGlobe
                checked: true
                text: "Free pan round the globe"
                onCheckedChanged: {
                    if (checked) {
                        chooseCameraControllerSample.setGlobeController();
                    }
                }
            }
        }
    }
}
