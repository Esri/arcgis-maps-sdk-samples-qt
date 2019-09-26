// [WriteFile Name=FindFeaturesUtilityNetwork, Category=Analysis]
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
    FindFeaturesUtilityNetworkSample {
        id: model
        mapView: view
    }

    Dialog {
        modal: true
        standardButtons: Dialog.Ok
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
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

        onAccepted: {
//            console.log("accepted :%1".arg(terminalSelection.currentIndex));
//            terminal = terminals[terminalSelection.currentIndex];
//            element = utilityNetwork.createElementWithArcGISFeature(feature, terminal);
//            console.log(element.objectId);
            model.multiTerminalIndex(terminalSelection.currentIndex);
        }

    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: false
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

        GridLayout {
            columns: 2
            rows: 2
            flow: GridLayout.LeftToRight
//        ColumnLayout {
            RadioButton {
                id: startingLocBtn
                checked: true
                text: qsTr("Add starting locations")
                onToggled: model.startingLocationsEnabled = checked;
            }

            RadioButton {
                id: barriersBtn
                text: qsTr("Add barriers")
                onToggled: model.startingLocationsEnabled = !checked;
            }

            Button {
                id: resetBtn
                text: qsTr("Reset")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: model.reset();
            }

            Button {
                id: traceBtn
                text: qsTr("Trace")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: model.trace();
            }
        }
    }
}
