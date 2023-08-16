// [WriteFile Name=BrowseBuildingFloors, Category=Maps]
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

<<<<<<< HEAD
    // Add a background to the column
    Rectangle {
        anchors.fill: col
        radius: 10
        border.width: 1
    }

    Column {
        id: col
=======
    Column {
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
        spacing: 15
        padding: 10

        ComboBox {
            id: floor_level
            width: 200
            model: ["Level 1", "Level 2", "Level 3"]
        }

        Button {
            id: select_floor
            width: 200
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Select Floor"
            onClicked: {
                model.selectFloor(floor_level.currentText);
            }
        }
    }
}
