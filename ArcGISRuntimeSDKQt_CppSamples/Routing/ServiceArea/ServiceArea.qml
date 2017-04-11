// [WriteFile Name=ServiceArea, Category=Routing]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

ServiceAreaSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        anchors.centerIn: editRow
        radius: 8 * scaleFactor
        height: editRow.height + (16 * scaleFactor)
        width: editRow.width + (16 * scaleFactor)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2 * scaleFactor
        opacity: 0.75
    }

    Row {
        id: editRow
        anchors {
            top: parent.top
            left: parent.left
            margins: 24 * scaleFactor
        }
        spacing: 8 * scaleFactor

        ComboBox {
            id: modeComboBox
            width: 100 * scaleFactor
            model: ["Facility", "Barrier"]

            onCurrentTextChanged: {
                if (currentText === "Facility")
                    setFacilityMode();
                else
                    setBarrierMode();
            }
        }

        Button {
            id: newBarrierButton
            visible: modeComboBox.currentText === "Barrier"
            text: "new barrier"
            enabled: visible

            onClicked: {
                newBarrier();
            }
        }
    }

    Rectangle {
        anchors.centerIn: solveRow
        radius: 8 * scaleFactor
        height: solveRow.height + (16 * scaleFactor)
        width: solveRow.width + (16 * scaleFactor)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2 * scaleFactor
        opacity: 0.75
    }

    Row {
        id: solveRow
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 32 * scaleFactor
        }
        spacing: 8 * scaleFactor

        Button {
            id: serviceAreasButton
            text: "Solve"
            enabled: !busy

            onClicked: setSolveServiceAreaMode();
        }

        Button {
            text: "Reset"
            width: serviceAreasButton.width
            enabled: !busy
            onClicked: {
                resetMode();
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: busy
    }

    MessageDialog {
        id: messageDialog
        title: "Route Error"
        text: message
        visible: text.length > 0
    }
}
