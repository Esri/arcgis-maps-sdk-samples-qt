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
import QtQuick.Controls 2.2
import Esri.Samples 1.0

ServiceAreaSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Rectangle {
            anchors.centerIn: solveRow
            radius: 8
            height: solveRow.height + (16)
            width: solveRow.width + (16)
            color: "lightgrey"
            border.color: "darkgrey"
            border.width: 2
            opacity: 0.75
        }

        Row {
            id: solveRow
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 15
            }
            spacing: 8

            Button {
                id: serviceAreasButton
                text: "Solve"
                enabled: !busy

                onClicked: solveServiceArea();
            }

            Button {
                text: "Reset"
                enabled: !busy
                onClicked: {
                    reset();
                }
            }
        }
    }

    Rectangle {
        anchors.centerIn: editRow
        radius: 8
        height: editRow.height + (16)
        width: editRow.width + (16)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2
        opacity: 0.75
    }

    Row {
        id: editRow
        anchors {
            top: parent.top
            left: parent.left
            margins: 24
        }
        spacing: 8

        ComboBox {
            id: modeComboBox
            model: ["Facility", "Barrier"]

            property int modelWidth: 0
            width: modelWidth + leftPadding + rightPadding + indicator.width

            onCurrentTextChanged: {
                if (currentText === "Facility")
                    setFacilityMode();
                else
                    setBarrierMode();
            }

            Component.onCompleted : {
                for (let i = 0; i < model.length; ++i) {
                    metrics.text = model[i];
                    modelWidth = Math.max(modelWidth, metrics.width);
                }
            }
            TextMetrics {
                id: metrics
                font: modeComboBox.font
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

    BusyIndicator {
        anchors.centerIn: parent
        running: busy
    }

    Dialog {
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Route Error"
        visible: text.length > 0
        property alias text : textLabel.text
        Text {
            id: textLabel
            text: message
        }
    }
}
