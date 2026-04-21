// [WriteFile Name=BrowseBuildingFloors, Category=Maps]
// [Legal]
// Copyright 2022 Esri.
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

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    BrowseBuildingFloorsSample {
        id: model
        mapView: view
    }

    // Add a background to the column
    Pane {
        anchors.fill: col
    }

    Column {
        id: col
        spacing: 15
        padding: 10

        ComboBox {
            id: floor_level
            width: 200
            model: [qsTr("Level 1"), qsTr("Level 2"), qsTr("Level 3")]
            onCurrentTextChanged: model.selectFloor(currentText);
        }
    }

}
