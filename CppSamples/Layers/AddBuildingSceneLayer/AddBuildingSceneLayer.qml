// [WriteFile Name=AddBuildingSceneLayer, Category=Layers]
// [Legal]
// Copyright 2025 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {

    LocalSceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: view.attributionTop
            }
            width: fullModelButton.width + 20
            height: fullModelButton.height + 20
            color: palette.base
            radius: 5
            visible: model.layerLoaded

            Switch {
                id: fullModelButton
                anchors.centerIn: parent
                text: qsTr("Full Model")
                checkable: true
                onCheckedChanged: model.setFullModelAndOverviewVisibility(checked)
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    AddBuildingSceneLayerSample {
        id: model
        localSceneView: view
    }


}

