// [WriteFile Name=TraceUtilityNetwork, Category=UtilityNetwork]
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
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    TraceUtilityNetworkSample {
        id: model
        mapView: view
    }

    Dialog {
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: model.terminalDialogVisisble

        ColumnLayout {
            Text {
                text: qsTr("Select the terminal for this junction.")
                Layout.alignment: Qt.AlignHCenter
            }

            ComboBox {
                id: terminalSelection
                model: ["High", "Low"]
                Layout.alignment: Qt.AlignHCenter
            }
        }

        onAccepted: model.multiTerminalIndex(terminalSelection.currentIndex);
    }

    Dialog {
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: model.dialogVisible

        Text {
            text: model.dialogText
            anchors.centerIn: parent
        }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: model.busy
    }

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

        // catch mouse signals from propagating to parent
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Column {

            GridLayout {
                columns: 2
                rows: 3
                flow: GridLayout.LeftToRight
                RadioButton {
                    id: startingLocBtn
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    checked: true
                    text: qsTr("Add starting location(s)")
                    onToggled: model.startingLocationsEnabled = checked;
                }

                RadioButton {
                    id: barriersBtn
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    text: qsTr("Add barrier(s)")
                    onToggled: model.startingLocationsEnabled = !checked;
                }

                Text {
                    text: qsTr("Trace Type:")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                }

                ComboBox {
                    id: traceTypes
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: ["Connected", "Subnetwork", "Upstream", "Downstream"]
                }

                Button {
                    id: resetBtn
                    text: qsTr("Reset")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    enabled: !busy.visible
                    onClicked: model.reset();
                }

                Button {
                    id: traceBtn
                    text: qsTr("Trace")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    enabled: !busy.visible
                    onClicked: model.trace(traceTypes.currentIndex);
                }
            }

            Text {
                // Displays fraction along edge or if a junction is selected
                text: model.junctionSelected ? qsTr("Junction selected") : qsTr("Fraction along edge: %1".arg(model.fractionAlongEdge.toFixed(6)))
                anchors.horizontalCenter: parent.horizontalCenter
            }

        }
    }
}
