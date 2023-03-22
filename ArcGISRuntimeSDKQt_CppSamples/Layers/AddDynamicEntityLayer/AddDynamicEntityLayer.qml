// [WriteFile Name=AddDynamicEntityLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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
    id: item1

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
    AddDynamicEntityLayerSample {
        id: model
        mapView: view
        onConnectionStatusChanged: {
            // Enable switch if data stream is successfully disconnected or connected
            statusSwitch.enabled = (["Disconnected", "Connected"].indexOf(model.connectionStatus) !== -1)
        }
    }

    Rectangle {
        id: dynamicEntityLayerOptions
        x: 330
        width: 300
        height: 147
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        Column {
            id: column
            height: 400
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 10

            Text {
                id: statusText
                text: "Status: " + model.connectionStatus
                font.pixelSize: 12
            }

            Switch {
                id: statusSwitch
                checked: true
                onCheckedChanged: {
                    model.enableDisableConnection();
                }
            }

            CheckBox {
                id: trackLinesBox
                text: "Track lines"
                checked: true
                onCheckedChanged: {
                    model.showTrackLines(checked);
                }
            }


            CheckBox {
                id: previousObservationsBox
                text: "Previous observations"
                checked: true
                onCheckedChanged: {
                    model.showPreviousObservations(checked);
                }
            }

            Text {
                id: observationsSliderText
                text: "Observations per track (" + observationsSlider.value + ")"
            }

            Slider {
                id: observationsSlider
                anchors.left: parent.left
                anchors.right: parent.right
                value: 5
                stepSize: 1
                from: 1
                to: 16
                onValueChanged: {
                    model.setObservationsPerTrack(value)
                }
            }




        }
    }
}
