// [WriteFile Name=ApplyMosaicRuleToRasters, Category=Layers]
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

Item {

    property var rasterRules: ["None","NorthWest","Center","ByAttribute","LockRaster"]

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            width: childrenRect.width
            height: childrenRect.height
            anchors {
                bottom: view.attributionTop
                horizontalCenter: view.horizontalCenter
            }
            color: "lightgrey"
            opacity: 0.8
            visible: model.rasterLoaded

            Column {
                width: childrenRect.width
                height: childrenRect.height

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Select Mosaic Rule")
//                    color: "black"
                }
                ComboBox {
                    id: rasterRulesComboBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: rasterRules
                    visible: model.rasterLoaded

                    onActivated: model.applyRasterRule(rasterRules[currentIndex]);
                }
            }
        }

//        ComboBox {
//            id: rasterRulesComboBox
//            anchors {
//                bottom: view.attributionTop
//                horizontalCenter: view.horizontalCenter
//            }
//            model: rasterRules
//            visible: model.rasterLoaded

//            onActivated: model.applyRasterRule(rasterRules[currentIndex]);
//        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ApplyMosaicRuleToRastersSample {
        id: model
        mapView: view
    }
}
