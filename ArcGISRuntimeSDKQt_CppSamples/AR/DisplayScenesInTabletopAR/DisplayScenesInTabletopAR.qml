// [WriteFile Name=DisplayScenesInTabletopAR, Category=AR]
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
import Esri.Samples 1.0
import Esri.ArcGISArToolkit 1.0

Item {

    ArcGISArView {
        id: arcGISArView
        anchors.fill: parent
        sceneView: view
        tracking: true
    }

    SceneView {
        id: view
        anchors.fill: parent

        Rectangle {
            anchors {
                bottom: view.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#88ffffff" // transparent white
            radius: 5
            visible: model.dialogVisible
            Text {
                anchors.centerIn: parent
                padding: 2
                font.bold: true
                text: qsTr("Touch screen to place the tabletop scene...")
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplayScenesInTabletopARSample {
        id: model
        arcGISArView: arcGISArView
        sceneView: view
    }
}
