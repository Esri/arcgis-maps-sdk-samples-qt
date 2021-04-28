// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.

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
    property var phases: ["A", "AB", "ABC", "AC", "B", "BC", "C", "DeEnergized", "Unknown"]

    property var selectedPhases: {
        "A": false,
        "AB": false,
        "ABC": false,
        "AC": false,
        "BC": false,
        "B": false,
        "C": false,
        "DeEnergized": false,
        "Unknown": false
    }

    property bool reportHasRun: false

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateLoadReportSample {
        id: sampleModel
    }

    Rectangle {
        id: rectangle
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        color: "white"
        width: grid.width
        height: contents.height

        ButtonGroup {
            id: checkBoxes
            exclusive: false
            checkState: parentBox.checkState
        }

        Column {
            id: contents
            anchors.fill: parent
            padding: 10
            spacing: 25

            Row {
                GridLayout {
                    id: grid
                    columns: 4
                    rowSpacing: 5

                    CheckBox { id: parentBox; checkState: checkBoxes.checkState }
                    Text { text: "Phase"; font.pointSize: 18; font.bold: true }
                    Text { text: "Total customers"; font.pointSize: 18; font.bold: true; }
                    Text { text: "Total load"; font.bold: true; font.pointSize: 18 }

                    CheckBox { id: checkA; onCheckedChanged: selectedPhases["A"] = !selectedPhases["A"]; ButtonGroup.group: checkBoxes }
                    Text { text: "A" }
                    Text { text: "A" in sampleModel.phaseCust ? sampleModel.phaseCust["A"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "A" in sampleModel.phaseLoad ? sampleModel.phaseLoad["A"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkAB; onCheckedChanged: selectedPhases["AB"] = !selectedPhases["AB"]; ButtonGroup.group: checkBoxes }
                    Text { text: "AB" }
                    Text { text: "AB" in sampleModel.phaseCust ? sampleModel.phaseCust["AB"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "AB" in sampleModel.phaseLoad ? sampleModel.phaseLoad["AB"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkABC; onCheckedChanged: selectedPhases["ABC"] = !selectedPhases["ABC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "ABC" }
                    Text { text: "ABC" in sampleModel.phaseCust ? sampleModel.phaseCust["ABC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "ABC" in sampleModel.phaseLoad ? sampleModel.phaseLoad["ABC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkAC; onCheckedChanged: selectedPhases["AC"] = !selectedPhases["AC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "AC" }
                    Text { text: "AC" in sampleModel.phaseCust ? sampleModel.phaseCust["AC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "AC" in sampleModel.phaseLoad ? sampleModel.phaseLoad["AC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkB; onCheckedChanged: selectedPhases["B"] = !selectedPhases["B"]; ButtonGroup.group: checkBoxes }
                    Text { text: "B" }
                    Text { text: "B" in sampleModel.phaseCust ? sampleModel.phaseCust["B"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "B" in sampleModel.phaseLoad ? sampleModel.phaseLoad["B"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkBC; onCheckedChanged: selectedPhases["BC"] = !selectedPhases["BC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "BC" }
                    Text { text: "BC" in sampleModel.phaseCust ? sampleModel.phaseCust["BC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "BC" in sampleModel.phaseLoad ? sampleModel.phaseLoad["BC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkC; onCheckedChanged: selectedPhases["C"] = !selectedPhases["C"]; ButtonGroup.group: checkBoxes }
                    Text { text: "C" }
                    Text { text: "C" in sampleModel.phaseCust ? sampleModel.phaseCust["C"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "C" in sampleModel.phaseLoad ? sampleModel.phaseLoad["C"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkDeEnergized; onCheckedChanged: selectedPhases["DeEnergized"] = !selectedPhases["DeEnergized"]; ButtonGroup.group: checkBoxes }
                    Text { text: "DeEnergized" }
                    Text { text: "DeEnergized" in sampleModel.phaseCust ? sampleModel.phaseCust["DeEnergized"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "DeEnergized" in sampleModel.phaseLoad ? sampleModel.phaseLoad["DeEnergized"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkUnknown; onCheckedChanged: selectedPhases["Unknown"] = !selectedPhases["Unknown"]; ButtonGroup.group: checkBoxes }
                    Text { text: "Unknown" }
                    Text { text: "Unknown" in sampleModel.phaseCust ? sampleModel.phaseCust["Unknown"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "Unknown" in sampleModel.phaseLoad ? sampleModel.phaseLoad["Unknown"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                }
            }

            Row {
                Button {
                    text: {
                        if (checkBoxes.checkState !== 0 || !reportHasRun) {
                            "Run Report";
                        } else {
                            "Reset";
                        }
                    }

                    enabled: ((reportHasRun || checkBoxes.checkState !== 0) && sampleModel.sampleStatus === 2) ? true : false

                    onClicked: {
                        let runPhases = [];
                        phases.forEach((phase) => {
                                           if (selectedPhases[phase])
                                                runPhases.push(phase)
                                       });
                        sampleModel.runReport(runPhases);

                        reportHasRun = runPhases.length === 0;
                    }
                }
            }

            Row {
                Text {
                    id: noticeText
                    text: {
                        switch (sampleModel.sampleStatus) {
                        case -1: // SampleError
                            "Error initializing sample";
                            break;
                        case 0: // SampleNotLoaded
                            "Sample initializing...";
                            break;
                        case 1: // SampleBusy
                            "Generating load report...";
                            break;
                        case 2: // SampleReady
                            if (checkBoxes.checkState === 0 && !reportHasRun) {
                                "Select phases to run the load report with";
                            } else {
                                "Tap the \"Run Report\" button to create the load report";
                            }
                            break;
                        default:
                            "";
                            break;
                        }
                    }
                }
            }
        }
    }
}
