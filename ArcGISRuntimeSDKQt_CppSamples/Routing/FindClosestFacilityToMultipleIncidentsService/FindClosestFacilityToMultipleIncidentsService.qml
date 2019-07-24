// [WriteFile Name=FindClosestFacilityToMultipleIncidentsService, Category=Routing]
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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.Samples 1.0


Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        // background for buttons
        Rectangle {
            anchors {
                margins: 5
                left: parent.left
                top: parent.top
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#000000"
            opacity: .70
            radius: 5

            // catch mouse signals from propagating to parent
            MouseArea {
                anchors.fill: parent
                onClicked: mouse.accepted = true
                onWheel: wheel.accepted = true
            }

            // column layout for solve and reset buttons
            ColumnLayout {
                Button {
                    id: solveButton
                    text: qsTr("Solve Routes")
                    Layout.margins: 2
                    Layout.fillWidth: true
                    enabled: closestFacilityModel.solveButton
                    onClicked: closestFacilityModel.solveRoute();
                }

                Button {
                    id: resetButton
                    text: qsTr("Reset")
                    Layout.margins: 2
                    Layout.fillWidth: true
                    enabled: closestFacilityModel.resetButton
                    onClicked: closestFacilityModel.resetGO();
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: closestFacilityModel.busy
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    FindClosestFacilityToMultipleIncidentsServiceSample {
        id: closestFacilityModel
        mapView: view
    }
}
