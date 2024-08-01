// [WriteFile Name=OfflineRouting, Category=Routing]
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

import QtQuick
import QtQuick.Controls
import Esri.Samples
import QtQuick.Layouts

Item {
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    RowLayout {
        anchors {
            left: parent.left
            top: parent.top
        }

        ComboBox {
            id: comboBox
            Layout.fillWidth: true
            model: routingModel.travelModeNames
            onCurrentIndexChanged: {
                routingModel.travelModeIndex = currentIndex;
                routingModel.findRoute();
            }

            // Add a background to the ComboBox
            Rectangle {
                anchors.fill: parent
                radius: 10
                // Make the rectangle visible if a dropdown indicator exists
                // An indicator only exists if a theme is set
                visible: parent.indicator
                border.width: 1
            }
        }

        Button {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Reset"
            onClicked: {
                routingModel.resetMap();
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    OfflineRoutingSample {
        id: routingModel
        mapView: view
    }
}
