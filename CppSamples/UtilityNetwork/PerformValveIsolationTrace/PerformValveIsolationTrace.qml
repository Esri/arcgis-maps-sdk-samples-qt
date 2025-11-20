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
            color: palette.base
            height: row.height * 3
            width: row.width * 1.05

            MouseArea {
                anchors.fill: parent
                onClicked: mouse => mouse.accepted = true
                onWheel: wheel => wheel.accepted = true
            }

            RowLayout {
                id: titleRow
                anchors {
                    left: row.left
                    top: parent.top
                    topMargin: 10
                }
                Rectangle {
                    color: backgroundRect.color
                    width: childrenRect.width
                    height: childrenRect.height
                    Label {
                        text: qsTr("Choose category for filter barrier:")
                        font.pixelSize: 14
                    }
                }
            }

            RowLayout {
                id: row
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: titleRow.bottom
                    topMargin: 10
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
                    text: qsTr("Trace")
                    onClicked: {
                        sampleModel.performTrace();
                    }
                    enabled: !sampleModel.tasksRunning
                }
                Button {
                    text: qsTr("Reset")
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
                    topMargin: 10
                }

                CheckBox {
                    text: qsTr("Include isolated features")
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
        visible: sampleModel.tasksRunning
    }

    Dialog {
        id: messageDialog
        visible: sampleModel.noResults
        standardButtons: Dialog.Ok
        anchors.centerIn: parent
        Label {
            text: qsTr("Isolation trace returned no elements.")
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    PerformValveIsolationTraceSample {
        id: sampleModel
        mapView: view
    }
}
