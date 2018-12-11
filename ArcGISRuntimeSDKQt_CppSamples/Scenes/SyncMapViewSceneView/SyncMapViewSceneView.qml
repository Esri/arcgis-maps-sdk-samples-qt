// [WriteFile Name=SyncMapViewSceneView, Category=Scenes]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Layouts 1.2
import Esri.Samples 1.0

Item {
    states: [
        State {
            name: "orientHorizontal"
            when: width > height
            PropertyChanges {
                target: layout
                flow: GridLayout.LeftToRight
            }
        },
        State {
            name: "orientVertical"
            when: width <= height
            PropertyChanges {
                target: layout
                flow: GridLayout.TopToBottom
            }
        }
    ]

    GridLayout {
        id: layout
        anchors.fill: parent
        SceneView {
            id: sceneView
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        MapView {
            id: mapView
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    SyncMapViewSceneViewSample {
        id: model
        sceneView: sceneView
        mapView: mapView
    }
}
