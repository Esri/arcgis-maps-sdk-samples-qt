// [WriteFile Name=RouteAroundBarriers, Category=Routing]
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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import QtQuick.Layouts 1.11

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        GridLayout {
            id: grid
            rows: 2
            columns: 1
            rowSpacing: 10
            columnSpacing: 2

            Row {
                id: buttonsRow

                spacing: 5
                padding: 5
                Button {
                    id: stopButton
                    text: "Stops"
                    onClicked: {
                        model.addStops = !model.addStops;
                        highlighted = !highlighted;
                    }
                }
                Button {
                    id: barrierButton
                    text: "Barriers"
                    onClicked: {
                        model.addBarriers = !model.addBarriers;
                        highlighted = !highlighted;
                    }
                }
                Button {
                    id: resetButton
                    text: "Reset"
                }
            }

            Column {
                spacing: 2

                CheckBox {
                    id: bestSequenceBox
                    text: "Find best sequence"
                }
                CheckBox {
                    id: firstStopBox
                    text: "Preserve first stop"
                }
                CheckBox {
                    id: lastStopBox
                    text: "Preserve last stop"
                }
            }
        }


        //        Rectangle {
        //            id: controlBox
        //            width: buttonsRow.width
        ////            anchors.centerIn: buttonsRow
        //            color: "lightgrey"
        //            border.color: "darkgrey"
        //            border.width: 2
        ////            opacity: 0.75

        //            Row {
        //                id: buttonsRow
        //                anchors {
        ////                    bottom: mapView.attributionTop
        //                    top: parent.top
        //                    horizontalCenter: parent.horizontalCenter
        //                    margins: 15
        //                }
        //                spacing: 5
        //                Button {
        //                    id: stopButton
        //                    text: "Stops"
        //                }
        //                Button {
        //                    id: barrierButton
        //                    text: "Barriers"
        //                }
        //                Button {
        //                    id: resetButton
        //                    text: "Reset"
        //                }
        //            }

        //            Column {
        //                top: buttonsRow.bottom
        //                CheckBox {
        //                    text: "Find best sequence"
        //                }
        //                CheckBox {
        //                    text: "Preserve first stop"
        //                }
        //                CheckBox {
        //                    text: "Preserve last stop"
        //                }
        //            }
        //        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    RouteAroundBarriersSample {
        id: model
        mapView: view
    }
}
