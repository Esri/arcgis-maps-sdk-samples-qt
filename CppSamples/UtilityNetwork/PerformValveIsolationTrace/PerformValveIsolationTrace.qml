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

    Rectangle {
        id: backgroundRect
        anchors {
            right: parent.right
            top: parent.top
            margins: 10
        }
        color: palette.base
        border {
            width: 1
            color: "darkgrey"
        }
        width: Math.min(controlsColumn.implicitWidth + 20, view.width - 20)
        height: controlsColumn.implicitHeight + 20

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ColumnLayout {
            id: controlsColumn
            anchors {
                fill: parent
                margins: 10
            }

            Label {
                text: qsTr("Choose category for filter barrier:")
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            ComboBox {
                id: comboBox
                enabled: !sampleModel.tasksRunning
                Layout.fillWidth: true
                model: sampleModel.categoriesList
                onCurrentIndexChanged: {
                    sampleModel.selectedIndex = currentIndex;
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Button {
                    text: qsTr("Trace")
                    Layout.fillWidth: true
                    enabled: !sampleModel.tasksRunning
                    onClicked: {
                        sampleModel.performTrace();
                    }
                }
                Button {
                    text: qsTr("Reset")
                    Layout.fillWidth: true
                    enabled: !sampleModel.tasksRunning
                    onClicked: {
                        sampleModel.performReset();
                    }
                }
            }

            CheckBox {
                text: qsTr("Include isolated features")
                enabled: !sampleModel.tasksRunning
                leftPadding: 0
                Layout.fillWidth: true
                onCheckedChanged: {
                    sampleModel.isolateFeatures = checked;
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
