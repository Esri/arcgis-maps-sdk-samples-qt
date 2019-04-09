// [WriteFile Name=GetElevationAtPoint, Category=Scenes]
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

import Esri.Samples 1.0
import QtQuick 2.6
import QtQuick.Controls 2.2

Item {

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    GetElevationAtPointSample {
        id: model
        sceneView: view
    }

    // Background rectangle for elevation display
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 30
        }

        color: Qt.rgba(0.2, 0.2, 0.2, 0.65);
        height: childrenRect.height

        // Elevation display text
        Text {
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            color: "white"
            padding: 15
            font.pointSize: 32

            // For vertical screens, keep the text within the bounding box via scaling down.
            scale: Math.min(1, (parent.width - padding) / contentWidth)

            // Display elevation value in meters, round to a single decimal place.
            text: "Elevation : " + Math.round(model.elevation * 10) / 10 + "m"
        }
    }

    // Display an indictor when the elevation query is running, since it might take a couple of seconds
    BusyIndicator {
        running: model.elevationQueryRunning === true
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) / 6.0
        height: width
    }

}
