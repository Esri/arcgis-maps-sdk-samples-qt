// [WriteFile Name=Surface_Placement, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Layouts 1.12

SurfacePlacementSample {
    width: 800
    height: 600

    // Create SceneView here, and create its Scene and GraphicsOverlay in C++ code
    //! [add a sceneView component]
    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            margins: 3
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        ColumnLayout {
            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
                text: qsTr("Draped mode")
                color: "white"
            }

            Switch {
                id: surfacePlacementMode
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2

                onCheckedChanged: changeDrapedVisibility();
            }

            Slider {
                id: zValueSlider
                from: 0
                value: 70
                to: 140
                onMoved: {
                    changeZValue(value);
                }

            }
        }
    }
    //! [add a sceneView component]
}
