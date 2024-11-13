// [WriteFile Name=DisplayDeviceLocationWithNmeaDataSources, Category=Maps]
// [Legal]
// Copyright 2021 Esri.

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
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    DisplayDeviceLocationWithNmeaDataSourcesSample {
        id: model
        mapView: view
    }

    Button {
        id: button
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 25
        }
        width: 200
        text: model.nmeaSimulationActive ? "RESET" : "START"
        onClicked: {
            model.nmeaSimulationActive = !model.nmeaSimulationActive;
            if (model.nmeaSimulationActive)
                model.start();
            else
                model.reset();
        }
    }
}
