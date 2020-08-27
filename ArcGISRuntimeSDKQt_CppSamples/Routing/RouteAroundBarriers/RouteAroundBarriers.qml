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
    readonly property int checkBoxPadding: 20

    ButtonGroup {
        buttons: buttonsRow.children
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
        ColumnLayout {
            spacing: 0
            Layout.alignment: Qt.AlignTop

            Rectangle {
                id: backBox
                z: 1
                Layout.alignment: Qt.AlignLeft
                Layout.margins: 3
                Layout.bottomMargin: 0
                width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 200 : 300
                height: childrenRect.height
                color: "lightgrey"

                GridLayout {
                    id: grid
                    anchors.horizontalCenter: parent.horizontalCenter
                    rows: 4
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
                            text: "Add stop"
                            checked: true
                            highlighted: checked
                            onClicked: {
                                checked = true;
                            }
                            onCheckedChanged: {
                                sampleModel.addStops = checked;
                            }
                        }
                        Button {
                            id: barrierButton
                            text: "Add barrier"
                            highlighted: checked;
                            onClicked: {
                                checked = true;
                            }
                            onCheckedChanged: {
                                sampleModel.addBarriers = checked;
                            }
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
                            }
                        }
                        CheckBox {
                            id: firstStopBox
                            text: "Preserve first stop"
                            leftPadding: checkBoxPadding
                            enabled: bestSequenceBox.checked
                            onCheckedChanged: {
                                sampleModel.preserveFirstStop = checked;
                                sampleModel.createAndDisplayRoute();
                            }
                        }
                        CheckBox {
                            id: lastStopBox
                            text: "Preserve last stop"
                            leftPadding: checkBoxPadding
                            enabled: bestSequenceBox.checked
                            onCheckedChanged: {
                                sampleModel.preserveLastStop = checked;
                                sampleModel.createAndDisplayRoute();
                            }
                        }
                    }

                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Button {
                            id: resetButton
                            text: "Reset"
                            onClicked: {
                                sampleModel.clearRouteAndGraphics();
                                sampleModel.clearDirections();
                            }
                        }
                    }

                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Button {
                            text: "Hide directions"
                            onClicked: {
                                if (text === "Hide directions") {
                                    directionsView.delegate = blankDelegate;
                                    text = "Show directions";
                                } else {
                                    directionsView.delegate = directionDelegate;
                                    text = "Hide directions";
                                }
                            }
                        }
                    }
                }
            }

            // Create window for displaying the route directions
            Rectangle {
                id: directionWindow
                Layout.alignment: Qt.AlignBottom
                Layout.topMargin: 0
                visible: true
                Layout.preferredWidth: backBox.width
                Layout.preferredHeight: 300
                Layout.margins: 3
                color: "lightgrey"

                ScrollView {
                    anchors.fill: parent

                    ListView {
                        id: directionsView
                        anchors {
                            fill: parent
                            margins: 5
                        }
                        header: Text {
                            text: "Directions:"
                            font.pixelSize: 22
                            bottomPadding: 8
                        }

                        // set the model to the DirectionManeuverListModel returned from the route
                        model: sampleModel.directions
                        delegate: directionDelegate
                    }
                }
            }
        }
    }

    Component {
        id: blankDelegate
        Rectangle {
            width: parent.width
            height: 35
            color: directionWindow.color
        }
    }

    Component {
        id: directionDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35
            color: directionWindow.color

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
                font.pixelSize: 14
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    RouteAroundBarriersSample {
        id: sampleModel
        mapView: view
    }
}
