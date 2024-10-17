// [WriteFile Name=FilterFeaturesInScene, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.
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
    // add a SceneView component
    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    FilterFeaturesInSceneSample {
        id: model
        sceneView: view
    }

    Button {
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }

        property int step: 0

        text: ["Load detailed buildings","Filter OSM buildings in extent","Reset scene"][step]
        onClicked: {
            switch (step) {
            case 0:
                // Show the detailed buildings scene layer and extent graphic
                model.loadScene();
                step++;
                break;

            case 1:
                // Hide buildings within the detailed building extent so they don't clip
                model.filterScene();
                step++;
                break;

            case 2:
                // Reset the scene to its original state
                model.reset();
                step = 0;
                break;

            default:
                step = 0;
                break;
            }
        }
    }
}
