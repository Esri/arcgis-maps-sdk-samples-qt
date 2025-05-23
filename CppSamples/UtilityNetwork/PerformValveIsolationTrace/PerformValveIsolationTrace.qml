// [WriteFile Name=PerformValveIsolationTrace, Category=UtilityNetwork]
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
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    ColumnLayout {
        anchors {
            left: parent.left
            top: parent.top
        }

        Rectangle {
            id: backgroundRect
            color: "#FBFBFB"
            height: childrenRect.height
            width: row.width * 1.25

            RowLayout {
                id: titleRow
                anchors {
                    left: row.left
                    top: parent.top
                }
                Rectangle {
                    color: backgroundRect.color
                    width: childrenRect.width
                    height: childrenRect.height
                    Text {
                        text: "Choose category for filter barrier:"
                        font.pixelSize: 14
                    }
                }
            }

            RowLayout {
                id: row
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: titleRow.bottom
                }
                ComboBox {
                    id: comboBox
                    enabled: !sampleModel.tasksRunning
                    Layout.minimumWidth: 200
                    model: sampleModel.categoriesList
                    onCurrentIndexChanged: {
                        sampleModel.selectedIndex = currentIndex;
                    }
                }
                Button {
                    text: "Trace"
                    onClicked: {
                        sampleModel.performTrace();
                    }
                    enabled: !sampleModel.tasksRunning
                }
                Button {
                    text: "Reset"
                    onClicked: {
                        sampleModel.performReset();
                    }
                    enabled: !sampleModel.tasksRunning
                }
            }

            RowLayout {
                id: checkBoxRow
                anchors {
                    top: row.bottom
                    left: row.left
                }

                CheckBox {
                    text: "Include isolated features"
                    enabled: !sampleModel.tasksRunning
                    leftPadding: 0
                    onCheckedChanged: {
                        sampleModel.isolateFeatures = checked;
                    }
                }
            }
        }
    }

    TerminalPickerView {
        id: terminalPickerView
        visible: sampleModel.terminals.length > 0
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: sampleModel.tasksRunning
    }

    Dialog {
        id: messageDialog
        visible: sampleModel.noResults
        title: "Isolation trace returned no elements."
        standardButtons: Dialog.Ok
        anchors.centerIn: parent
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    PerformValveIsolationTraceSample {
        id: sampleModel
        mapView: view
    }
}
