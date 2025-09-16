// [WriteFile Name=ToggleBetweenFeatureRequestModes, Category=Features]
// [Legal]
// Copyright 2025 Esri.

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

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ToggleBetweenFeatureRequestModesSample {
        id: model
        mapView: view
    }

    // Add a background to the column
    Rectangle {
        anchors.fill: column
        radius: 10
        border.width: 1
    }

    Column {
        id: column
        spacing: 15
        padding: 10

        ComboBox {
            id: request_mode
            width: 200
            model: ["Cache", "No Cache", "Manual Cache"]
            onCurrentTextChanged: {
                model.setFeatureRequestMode(currentIndex+1)
            }
        }

        Button {
            id: populate
            width: 200
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Populate"
            enabled: request_mode.currentIndex === 2
            onClicked: {
                model.fetchCacheManually();
            }
        }
    }
}
