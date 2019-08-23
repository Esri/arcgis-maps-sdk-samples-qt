// [WriteFile Name=FeatureLayerChangeRenderer, Category=Features]
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

FeatureLayerChangeRendererSample {
    id: changeRendererSample
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Row {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 5
            bottomMargin: 25
        }
        spacing: 5

        // button to change renderer
        Button {
            text: "Change Renderer"
            enabled: changeRendererSample.layerInitialized
            onClicked: {
                // Call C++ invokable function to change the layer's renderer
                changeRendererSample.changeRenderer();
            }
        }
    }
}
