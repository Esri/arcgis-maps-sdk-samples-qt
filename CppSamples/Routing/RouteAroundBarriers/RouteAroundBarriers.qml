// [WriteFile Name=RouteAroundBarriers, Category=Routing]
// [Legal]
// Copyright 2020 Esri.
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
import QtQuick.Layouts

Item {
    id: rootItem
    readonly property int checkBoxPadding: 20

    ButtonGroup {
        buttons: buttonsRow.children
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        ColumnLayout {
            spacing: 0
            Layout.alignment: Qt.AlignTop

            Rectangle {
                id: backBox
                z: 1
                Layout.alignment: Qt.AlignLeft
                Layout.margins: 3
                Layout.bottomMargin: 0
                implicitWidth: grid.implicitWidth
                height: childrenRect.height
                color: palette.base

                MouseArea {
                    anchors.fill: parent
                    onClicked: mouse => mouse.accepted = true
                    onWheel: wheel => wheel.accepted = true
                }

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

                        readonly property real buttonWidth: Math.max(stopButton.implicitWidth, barrierButton.implicitWidth)

                        Button {
                            id: stopButton
                            width: buttonsRow.buttonWidth
                            text: qsTr("Add stop")
                            checked: true
                            highlighted: checked
                            onClicked: checked = true
                            onCheckedChanged: sampleModel.addStops = checked
                        }
                        Button {
                            id: barrierButton
                            width: buttonsRow.buttonWidth
                            text: qsTr("Add barrier")
                            highlighted: checked
                            onClicked: checked = true
                            onCheckedChanged: sampleModel.addBarriers = checked
                        }
                    }

                    Column {
                        spacing: 8
                        Layout.alignment: Qt.AlignHCenter
                        CheckBox {
                            id: bestSequenceBox
                            text: qsTr("Find best sequence")
                            onCheckedChanged: {
                                sampleModel.findBestSequence = checked;
                                sampleModel.createAndDisplayRoute();
                            }
                        }
                        CheckBox {
                            id: firstStopBox
                            text: qsTr("Preserve first stop")
                            leftPadding: checkBoxPadding
                            enabled: bestSequenceBox.checked
                            onCheckedChanged: {
                                sampleModel.preserveFirstStop = checked;
                                sampleModel.createAndDisplayRoute();
                            }
                        }
                        CheckBox {
                            id: lastStopBox
                            text: qsTr("Preserve last stop")
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
                            text: qsTr("Reset")
                            onClicked: {
                                sampleModel.clearRouteAndGraphics();
                                sampleModel.clearDirections();
                            }
                        }
                    }

                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.bottomMargin: 10
                        Button {
                            id: toggleDirectionsButton
                            property bool userWantsVisible: false
                            enabled: sampleModel.directions !== null
                            text: directionWindow.visible ? qsTr("Hide directions") : qsTr("Show directions")
                            onClicked: userWantsVisible = !userWantsVisible
                        }
                    }
                }
            }

            // Create window for displaying the route directions
            Rectangle {
                id: directionWindow
                Layout.alignment: Qt.AlignBottom
                Layout.topMargin: 0
                visible: toggleDirectionsButton.userWantsVisible && sampleModel.directions !== null
                Layout.preferredWidth: backBox.width
                Layout.preferredHeight: 300
                Layout.margins: 3
                color: palette.base

                ScrollView {
                    anchors.fill: parent

                    ListView {
                        id: directionsView
                        implicitHeight: contentHeight
                        clip: true
                        anchors {
                            fill: parent
                            margins: 5
                        }
                        header: Label {
                            text: qsTr("Directions:")
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
        id: directionDelegate
        Rectangle {
            id: rect
            width: parent ? parent.width : 0
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
                color: palette.mid
                height: 1
            }

            Label {
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

    // Declare the C++ instance which creates the map etc. and supply the view
    RouteAroundBarriersSample {
        id: sampleModel
        mapView: view
    }
}
