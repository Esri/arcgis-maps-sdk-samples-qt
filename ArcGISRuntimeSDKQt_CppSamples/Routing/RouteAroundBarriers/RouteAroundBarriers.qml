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

    property int checkBoxPadding: 20

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: backBox
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"
            opacity: 0.8
            radius: 5


            GridLayout {
                id: grid
                rows: 3
                columns: 1
                rowSpacing: 10
                columnSpacing: 2

                Row {
                    id: buttonsRow
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 5
                    padding: 5
                    Button {
                        id: stopButton
                        text: "Stops"
                        onClicked: {
                            sampleModel.addStops = !sampleModel.addStops;
                            highlighted = !highlighted;
                        }
                    }
                    Button {
                        id: barrierButton
                        text: "Barriers"
                        onClicked: {
                            sampleModel.addBarriers = !sampleModel.addBarriers;
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
                        onCheckedChanged: {
                            sampleModel.findBestSequence = checked;
                            sampleModel.createAndDisplayRoute();
                            console.log("called from checkbox");
                        }
                    }
                    CheckBox {
                        id: firstStopBox
                        text: "Preserve first stop"
                        leftPadding: checkBoxPadding
                        onCheckedChanged: {
                            sampleModel.preserveFirstStop = checked;
                        }
                    }
                    CheckBox {
                        id: lastStopBox
                        text: "Preserve last stop"
                        leftPadding: checkBoxPadding
                        onCheckedChanged: {
                            sampleModel.preserveLastStop = checked;
                        }
                    }
                }

                Row {
                    Text {text: " "}
                    ListView {
                        id: directionsView
                        header: Text {
                                height: 40
                                text: "Directions: "
                                font.pixelSize: 22
                            }

                        model: sampleModel.directions
                        delegate: directionDelegate
                        visible: true
                    }
                }
            }

            Component {
                id: directionDelegate
                Rectangle {
                    id: rect
                    width: parent.width
                    height: 35
                    color: backBox.color

                    Rectangle {
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                            topMargin: -8
                            leftMargin: 20
                            rightMargin: 20
                        }
                        color: "darkgrey"
                        height: 1
                    }

                    Text {
                        text: directionText
                        anchors {
                            fill: parent
                            leftMargin: 5
                        }
                        elide: Text.ElideRight
                        font.pixelSize: 10
                    }
                }
            }
        }



    }







    // Declare the C++ instance which creates the scene etc. and supply the view
    RouteAroundBarriersSample {
        id: sampleModel
        mapView: view
    }
}
