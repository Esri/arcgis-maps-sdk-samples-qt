// [WriteFile Name=DisplayFeatureLayers, Category=Layers]
// [Legal]
// Copyright 2022 Esri.

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
import QtQuick.Layouts
import Esri.Samples

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    DisplayFeatureLayersSample {
        id: model
        mapView: view
    }

    Rectangle {
        id: layerSelectRectangle
        anchors {
            top: parent.top
            left: parent.left
            margins: 5
        }
        width: 225
        height: column.height + 10
        border.color: "black"
        ColumnLayout {
            id: column
            anchors.centerIn: parent
            Text {
                text: "Feature Layer Mode"
            }
            ComboBox {
                implicitWidth: layerSelectRectangle.width - 10
                model: ["Geodatabase", "Geopackage", "Portal Item", "Service Feature Table", "Shapefile"]
                currentIndex: 3
                onCurrentTextChanged: {
                    model.setLayerVisibility(currentIndex);
                }
            }
        }
    }
}
