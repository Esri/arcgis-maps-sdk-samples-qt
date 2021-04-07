// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
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
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {

    property var phaseVisible: {
        "A": true,
        "AB": false,
        "ABC": false,
        "AC": false,
        "BC": true,
        "B": true,
        "C": true,
        "DeEnergized": false,
        "Unknown": false
    }

    property var selectedPhases: []

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateLoadReportSample {
        id: sampleModel
        mapView: view
    }

    Rectangle {
        color: "white"
        width: contents.width
        height: contents.height
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }
        Column {
            id: contents
            spacing: 25

            Row {
                ComboBox {
                    id: phaseSelect
                    model: [
                        "A",
                        "AB",
                        "ABC",
                        "AC",
                        "B",
                        "BC",
                        "C",
                        "DeEnergized",
                        "Unknown"
                    ]
                }

                Button {
                    text: "Add Phase"
                    onClicked: {
                        selectedPhases.push(phaseSelect.currentValue);
                        console.log(phaseSelect.currentValue);
                        sampleModel.addPhase(phaseSelect.currentValue);
                        phaseVisible[phaseSelect.currentValue] = true;
                        console.log(phaseVisible[phaseSelect.currentValue]);
                        grid.update();
                    }
                }

                Button {
                    text: "Run Report"
                    onClicked: {
                        sampleModel.runReport(selectedPhases);
                    }
                }
            }

            Row {
                GridLayout {
                    id: grid
                    columns: 3

                    Text { text: "Phase"; }
                    Text { text: "Total customers"; }
                    Text { text: "Total load"; }

                    Text { text: "A"; visible: phaseVisible[text] }
                    Text { text: sampleModel.phaseCust["A"]; visible: phaseVisible["A"] }
                    Text { text: sampleModel.phaseLoad["A"]; visible: phaseVisible["A"] }

                    Text { text: "AB"; visible: phaseVisible["AB"] }
                    Text { text: sampleModel.phaseCust["AB"]; visible: phaseVisible["AB"] }
                    Text { text: sampleModel.phaseLoad["AB"]; visible: phaseVisible["AB"] }

                    Text { text: "ABC"; visible: phaseVisible["ABC"] }
                    Text { text: sampleModel.phaseCust["ABC"]; visible: phaseVisible["ABC"] }
                    Text { text: sampleModel.phaseLoad["ABC"]; visible: phaseVisible["ABC"] }

                    Text { text: "AC"; visible: phaseVisible["AC"] }
                    Text { text: sampleModel.phaseCust["AC"]; visible: phaseVisible["AC"] }
                    Text { text: sampleModel.phaseLoad["AC"]; visible: phaseVisible["AC"] }

                    Text { text: "BC"; visible: phaseVisible["BC"] }
                    Text { text: sampleModel.phaseCust["BC"]; visible: phaseVisible["BC"] }
                    Text { text: sampleModel.phaseLoad["BC"]; visible: phaseVisible["BC"] }

                    Text { text: "B"; visible: phaseVisible["B"] }
                    Text { text: sampleModel.phaseCust["B"]; visible: phaseVisible["B"] }
                    Text { text: sampleModel.phaseLoad["B"]; visible: phaseVisible["B"] }

                    Text { text: "C"; visible: phaseVisible["C"] }
                    Text { text: sampleModel.phaseCust["C"]; visible: phaseVisible["C"] }
                    Text { text: sampleModel.phaseLoad["C"]; visible: phaseVisible["C"] }

                    Text { text: "DeEngergized"; visible: phaseVisible["DeEnergized"] }
                    Text { text: sampleModel.phaseCust["DeEnergized"]; visible: phaseVisible["DeEnergized"] }
                    Text { text: sampleModel.phaseLoad["DeEnergized"]; visible: phaseVisible["DeEnergized"] }

                    Text { text: "Unknown"; visible: phaseVisible["Unknown"] }
                    Text { text: sampleModel.phaseCust["Unknown"]; visible: phaseVisible["Unknown"] }
                    Text { text: sampleModel.phaseLoad["Unknown"]; visible: phaseVisible["Unknown"] }
                }
            }
        }
    }
}
