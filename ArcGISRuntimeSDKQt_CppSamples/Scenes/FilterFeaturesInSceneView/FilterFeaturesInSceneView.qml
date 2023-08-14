// [WriteFile Name=FilterFeaturesInSceneView, Category=Scenes]
// [Legal]
// Copyright 2023 Esri.

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
    FilterFeaturesInSceneViewSample {
        id: model
        sceneView: view
    }

    Button {
        id: loadSceneButton
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        visible: true

        text: "Load detailed buildings"
        onClicked: {
            model.loadScene();
            loadSceneButton.visible = false;
            filterSceneButton.visible = true;
        }
    }

    Button {
        id: filterSceneButton
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        visible: false

        text: "Filter OSM buildings in extent"
        onClicked: {
            model.filterScene();
            filterSceneButton.visible = false;
            resetButton.visible = true;
        }
    }

    Button {
        id: resetButton
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        visible: false

        text: "Reset scene"
        onClicked: {
            model.reset();
            resetButton.visible = false;
            loadSceneButton.visible = true;
        }
    }
}
