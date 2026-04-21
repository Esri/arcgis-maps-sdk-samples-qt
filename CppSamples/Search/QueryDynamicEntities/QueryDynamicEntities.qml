// [WriteFile Name=QueryDynamicEntities, Category=Search]
// [Legal]
// Copyright 2026 Esri.

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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

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
    QueryDynamicEntitiesSample {
        id: model
        mapView: view
    }

    Rectangle {
        id: controlBox
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }
        width: Math.min(column.implicitWidth, parent.width * 0.9)
        height: column.implicitHeight
        color: palette.base
        opacity: 1
        border.color: "black"
        border.width: 1
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
        }
        Column {
            id: column
            leftPadding: 20
            topPadding: 10
            rightPadding: 20
            bottomPadding: 10
            spacing: 10

            GridLayout {
                id: queryGrid
                columns: 2
                columnSpacing: 8
                width: Math.min(controlBox.width * 0.9, controlBox.width)
                Label {
                    text: qsTr("Query Flights")
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: (queryGrid.width - queryGrid.columnSpacing) * 0.3
                }
                ComboBox {
                    id: querySelector
                    model: ["", "Within 15 Miles of PHX", "Arriving in PHX", "Flight Number"]
                    currentIndex: 0
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: (queryGrid.width - queryGrid.columnSpacing) * 0.7
                    onActivated: {
                        if (!currentText || currentText.length === 0) {
                            flightsCombo.currentIndex = -1
                            detailsText.text = ""
                            return
                        }
                        // Clear prior UI state when switching queries
                        flightsCombo.currentIndex = -1
                        detailsText.text = ""
                        model.handleQuerySelection(currentText)
                    }
                }
            }

            // Query results dropdown
            GridLayout {
                id: resultsGrid
                visible: querySelector.currentText === "Within 15 Miles of PHX" || querySelector.currentText === "Arriving in PHX"
                columns: 2
                columnSpacing: 8
                width: Math.min(controlBox.width * 0.9, controlBox.width)
                Label {
                    text: qsTr("Query Results")
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: (resultsGrid.width - resultsGrid.columnSpacing) * 0.3
                }
                ComboBox {
                    id: flightsCombo
                    visible: resultsGrid.visible
                    model: model.resultsModel
                    textRole: "flightNumber"
                    Layout.preferredWidth: (resultsGrid.width - resultsGrid.columnSpacing) * 0.7
                    currentIndex: -1
                    onActivated: {
                        detailsText.text = model.flightDetailsForRow(currentIndex)
                    }
                }
            }

            // Flight number query input
            GridLayout {
                id: searchGrid
                visible: querySelector.currentText === "Flight Number"
                columns: 2
                columnSpacing: 8
                width: Math.min(controlBox.width * 0.9, controlBox.width)
                TextField {
                    id: flightNumberField
                    placeholderText: qsTr("(Eg: Flight_1107)")
                    Layout.preferredWidth: (searchGrid.width - searchGrid.columnSpacing) * 0.6
                    onAccepted: runButton.clicked()
                }
                Button {
                    id: runButton
                    text: qsTr("Search")
                    Layout.preferredWidth: (searchGrid.width - searchGrid.columnSpacing) * 0.4
                    onClicked: {
                        detailsText.text = ""
                        model.runFlightNumberQuery(flightNumberField.text)
                    }
                }
            }

            // Details text shown when a flight is selected or for flight-number search
            Label {
                id: detailsText
                visible: (flightsCombo.visible && flightsCombo.currentIndex >= 0) || (querySelector.currentText === "Flight Number" && model.resultsCount > 0)
                text: ""
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                width: Math.min(controlBox.width * 0.9, controlBox.width)
            }

            // Update UI when results arrive
            Connections {
                target: model
                function onResultsModelChanged() {
                    if (querySelector.currentText === "Flight Number") {
                        // Show first result's details if available; empty otherwise
                        detailsText.text = model.flightDetailsForRow(0)
                    }
                }
            }

            // React to live data changes in the list model so details refresh
            Connections {
                target: model.resultsModel
                // Fired when one or more roles change for rows in the model
                function onDataChanged(topLeft, bottomRight, roles) {
                    if (flightsCombo.visible && flightsCombo.currentIndex >= 0) {
                        detailsText.text = model.flightDetailsForRow(flightsCombo.currentIndex)
                    } else if (querySelector.currentText === "Flight Number") {
                        detailsText.text = model.flightDetailsForRow(0)
                    }
                }
                function onRowsInserted(parent, start, end) {
                    if (querySelector.currentText === "Flight Number") {
                        detailsText.text = model.flightDetailsForRow(0)
                    }
                }
            }
        }
    }
}
