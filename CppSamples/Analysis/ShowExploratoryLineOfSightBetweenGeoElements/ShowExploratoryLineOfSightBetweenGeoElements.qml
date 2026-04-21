// [WriteFile Name=ShowExploratoryLineOfSightBetweenGeoElements, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {
    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Rectangle {
            anchors {
                margins: 5
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            color: palette.base
            radius: 10
            width: 80
            height: parent.height * 0.5

            Column {
                anchors {
                    fill: parent
                    margins: 10
                }
                spacing: 10

                Label {
                    text: "Height: " + Math.round(heightSlider.value) + " m"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 14
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Slider {
                    id: heightSlider
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: parent.height - parent.spacing - 40
                    width: 40
                    from: 150
                    to: 300
                    stepSize: 10
                    value: model.heightZ
                    orientation: Qt.Vertical
                    onMoved: {
                        model.heightZ = value;
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ShowExploratoryLineOfSightBetweenGeoElementsSample {
        id: model
        sceneView: view
    }
}
