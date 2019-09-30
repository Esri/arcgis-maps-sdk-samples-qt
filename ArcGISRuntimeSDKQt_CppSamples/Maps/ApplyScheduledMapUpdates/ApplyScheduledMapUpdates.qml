// [WriteFile Name=ApplyScheduledMapUpdates, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ApplyScheduledMapUpdatesSample {
        id: model
        mapView: view

        onUpdateUi: {
            downloadUpdatesButton.enabled = enabled;
            availabilityText.text = text;
            availabilityDetailsText.text = detailedText;
            busyIndicator.visible = false;
        }
    }

    Rectangle {
        anchors.fill: controlColumn
        anchors.margins: -5
        color: "#404040"
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Button {
            id: downloadUpdatesButton
            text: "Apply Scheduled Updates"
            enabled: false

            onClicked: {
                busyIndicator.visible = true;
                model.updateMap();
            }
        }

        Text {
            id: availabilityText
            color: "white"
        }

        Text {
            id: availabilityDetailsText
            color: "white"
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
    }
}
