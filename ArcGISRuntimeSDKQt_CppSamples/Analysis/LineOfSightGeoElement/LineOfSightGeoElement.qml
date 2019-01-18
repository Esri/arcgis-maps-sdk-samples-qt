// [WriteFile Name=LineOfSightGeoelement, Category=Analysis]
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
import Esri.Samples 1.0

Item {    
    SceneView {
        id: view
        anchors.fill: parent
        Slider {
            id: heightSlider
            anchors {
                margins: 5
                left: parent.left
                top: parent.top
                bottom: view.attributionTop
            }
            from: 150
            to: 300
            value: model.heightZ
            orientation: Qt.Vertical
            onMoved: {
                model.heightZ = value;
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    LineOfSightGeoelementSample {
        id: model
        sceneView: view
    }
}
