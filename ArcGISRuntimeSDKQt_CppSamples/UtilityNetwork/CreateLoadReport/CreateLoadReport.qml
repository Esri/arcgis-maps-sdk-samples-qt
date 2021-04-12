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

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateLoadReportSample {
        id: sampleModel
    }

    Rectangle {
        id: rectangle
        color: "white"
        width: contents.width
        height: contents.height
        anchors.fill: parent

        Column {

            id: contents
            anchors.fill: parent
            padding: 10
            spacing: 25

            Row {
                GridLayout {
                    id: grid
                    columns: 4

                    Text {}
                    Text { text: "Phase"; font.pointSize: 18; font.bold: true }
                    Text { text: "Total customers"; font.pointSize: 18; font.bold: true; }
                    Text { text: "Total load"; font.bold: true; font.pointSize: 18 }

                    CheckBox { onCheckedChanged: selectedPhases["A"] = !selectedPhases["A"]}
                    Text { text: "A" }
                    Text { text: sampleModel.phaseCust["A"] ? sampleModel.phaseCust["A"] : 0 }
                    Text { text: sampleModel.phaseLoad["A"] ? sampleModel.phaseLoad["A"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["AB"] = !selectedPhases["AB"]}
                    Text { text: "AB" }
                    Text { text: sampleModel.phaseCust["AB"] ? sampleModel.phaseCust["AB"] : 0 }
                    Text { text: sampleModel.phaseLoad["AB"] ? sampleModel.phaseLoad["AB"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["ABC"] = !selectedPhases["ABC"]}
                    Text { text: "ABC" }
                    Text { text: sampleModel.phaseCust["ABC"] ? sampleModel.phaseCust["ABC"] : 0 }
                    Text { text: sampleModel.phaseLoad["ABC"] ? sampleModel.phaseLoad["ABC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["AC"] = !selectedPhases["AC"]}
                    Text { text: "AC" }
                    Text { text: sampleModel.phaseCust["AC"] ? sampleModel.phaseCust["AC"] : 0 }
                    Text { text: sampleModel.phaseLoad["AC"] ? sampleModel.phaseLoad["AC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["B"] = !selectedPhases["B"]}
                    Text { text: "B" }
                    Text { text: sampleModel.phaseCust["B"] ? sampleModel.phaseCust["B"] : 0 }
                    Text { text: sampleModel.phaseLoad["B"] ? sampleModel.phaseLoad["B"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["BC"] = !selectedPhases["BC"]}
                    Text { text: "BC" }
                    Text { text: sampleModel.phaseCust["BC"] ? sampleModel.phaseCust["BC"] : 0 }
                    Text { text: sampleModel.phaseLoad["BC"] ? sampleModel.phaseLoad["BC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["C"] = !selectedPhases["C"]}
                    Text { text: "C" }
                    Text { text: sampleModel.phaseCust["C"] ? sampleModel.phaseCust["C"] : 0 }
                    Text { text: sampleModel.phaseLoad["C"] ? sampleModel.phaseLoad["C"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["DeEnergized"] = !selectedPhases["DeEnergized"]}
                    Text { text: "DeEnergized" }
                    Text { text: sampleModel.phaseCust["DeEnergized"] ? sampleModel.phaseCust["DeEnergized"] : 0 }
                    Text { text: sampleModel.phaseLoad["DeEnergized"] ? sampleModel.phaseLoad["DeEnergized"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["Unknown"] = !selectedPhases["Unknown"]}
                    Text { text: "Unknown" }
                    Text { text: sampleModel.phaseCust["Unknown"] ? sampleModel.phaseCust["Unknown"] : 0 }
                    Text { text: sampleModel.phaseLoad["Unknown"] ? sampleModel.phaseLoad["Unknown"] : 0 }
                }
            }

            Row {
                Button {
                    text: "Run Report"
                    onClicked: {
                        let runPhases = [];
                        phases.forEach((phase) => {
                                           if (selectedPhases[phase])
                                           runPhases.push(phase)
                                       });
                        sampleModel.runReport(runPhases);
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
