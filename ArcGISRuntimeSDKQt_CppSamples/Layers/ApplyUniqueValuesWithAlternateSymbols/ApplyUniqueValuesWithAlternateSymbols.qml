// [WriteFile Name=ApplyUniqueValuesWithAlternateSymbols, Category=Layers]
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
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }
    Rectangle {
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .75
        radius: 5

        ColumnLayout {
            Text {
                color: "#ffffff"
                text: "Current scale: 1:" + Math.round(model.currentScale)
                Layout.fillWidth: true
                Layout.margins: 3
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
            }
            Button {
                text: qsTr("Reset Viewpoint")
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
                Layout.margins: 3
                Layout.fillWidth: true
                onClicked: model.resetViewpoint();
            }
        }
    }
    // Declare the C++ instance which creates the scene etc. and supply the view
    ApplyUniqueValuesWithAlternateSymbolsSample {
        id: model
        mapView: view
    }
}
