// [WriteFile Name=DisplayRouteLayer, Category=Routing]
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
import QtQuick.Controls as Controls
import Esri.Samples
import QtQuick.Layouts

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Controls.Button {
            id: directionsButton
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                margins: 40
            }
            text: "Directions"
            enabled: false
            onClicked: popup.open();
        }

        Controls.Popup {
            id: popup
            anchors.centerIn: Controls.Overlay.overlay
            width: 300
            height: 320
            focus: true
            contentItem: Controls.ScrollView {
                contentWidth: parent.width - 30
                Text {
                    width: parent.width
                    text: model.directions
                    wrapMode: Text.WordWrap
                }
                clip: true
            }
            opacity: .9
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    DisplayRouteLayerSample {
        id: model
        mapView: view

        onEnableDirectionsButton: {
            directionsButton.enabled = true;
        }
    }
}
