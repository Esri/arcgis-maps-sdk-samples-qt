// [WriteFile Name=AddDynamicEntityLayer, Category=Layers]
// [Legal]
// Copyright 2023 Esri.
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
import Esri.ArcGISRuntime.Toolkit

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
    AddDynamicEntityLayerSample {
        id: model
        mapView: view
        onConnectionStatusChanged: {
            // Enable switch if data stream is successfully disconnected or connected
            statusSwitch.enabled = (["Disconnected", "Connected"].indexOf(model.connectionStatus) !== -1)
        }
    }

    Callout {
        id: callout
        height: 60
        calloutData: view.calloutData
        accessoryButtonVisible: false
    }

    Rectangle {
        id: dynamicEntityLayerOptions
        anchors {
            top: parent.top
            right: parent.right
            margins: 10
        }
        width: (300 < parent.width - 20) ? 300 : parent.width - 20
        height: column.height + 20
        border.width: 1

        // Catch mouse signals so they don't propagate to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        Column {
            id: column
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
            }

            Text {
                id: statusText
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Status: " + model.connectionStatus
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
            }

            Switch {
                id: statusSwitch
                anchors.horizontalCenter: parent.horizontalCenter
                checked: model.connectionStatus !== "Disconnected"
                onCheckedChanged: {
                    model.enableDisableConnection();
                    // Calls either of the following depending on the state
                    // m_dynamicEntityDataSource->connectDataSource();
                    // m_dynamicEntityDataSource->disconnectDataSource();
                }
            }

            CheckBox {
                id: trackLinesBox
                text: "Track lines"
                checked: true
                onCheckedChanged: {
                    model.showTrackLines(checked);
                    // Calls m_dynamicEntityLayer->trackDisplayProperties()->setShowTrackLine(showTrackLines);
                }
            }


            CheckBox {
                id: previousObservationsBox
                text: "Previous observations"
                checked: true
                onCheckedChanged: {
                    model.showPreviousObservations(checked);
                    // Calls m_dynamicEntityLayer->trackDisplayProperties()->setShowPreviousObservations(showPreviousObservations);
                }
            }

            Text {
                id: observationsSliderText
                topPadding: 10
                text: "Observations per track (" + observationsSlider.value + ")"
            }

            Slider {
                id: observationsSlider
                anchors {
                    left: parent.left
                    right: parent.right
                }
                value: 5
                stepSize: 1
                from: 1
                to: 16
                onValueChanged: {
                    model.setObservationsPerTrack(value)
                    // Calls m_dynamicEntityLayer->trackDisplayProperties()->setMaximumObservations(observationsPerTrack);
                }
            }

            Button {
                id: purgeButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Purge all observations"
                onClicked: {
                    callout.visible = false;
                    model.purgeAllObservations();
                    // Calls m_dynamicEntityDataSource->purgeAll();
                }
            }
        }
    }
}
