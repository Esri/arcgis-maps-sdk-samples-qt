// [WriteFile Name=ValidateUtilityNetworkTopology, Category=UtilityNetwork]
// [Legal]
// Copyright 2024 Esri.
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
import QtQuick.Layouts
import Esri.Samples
import QtQuick.Controls.Material

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
    ValidateUtilityNetworkTopologySample {
        id: model
        mapView: view
    }

    Rectangle {
        id: backBox
        anchors {
            right: parent.right
            top: parent.top
            margins: 20
        }
        width: layout.width
        height: layout.height
        color: "white"
        opacity: 1
        border.color: "black"
        border.width: 1
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
        }
        ColumnLayout {
            id: layout
            Column {
                id: column
                leftPadding: 20
                topPadding: 10
                rightPadding: 20
                bottomPadding: 10
                width: grid.width
                spacing: 10
                GridLayout {
                    id: grid
                    flow: GridLayout.LeftToRight

                    Button {
                        id: stateBtn
                        text: "Get State"
                        Layout.row: 0
                        Layout.column: 0
                        onClicked: model.onGetState()
                        Layout.preferredWidth: clearBtn.width
                        enabled: model.stateButtonAvailability
                    }
                    Button {
                        id: validateBtn
                        text: "Validate"
                        Layout.row: 0
                        Layout.column: 1
                        enabled: model.validateButtonAvailability
                        onClicked: model.onValidate()
                        Layout.preferredWidth: clearBtn.width
                    }
                    Button {
                        id: traceBtn
                        text: "Trace"
                        Layout.row: 1
                        Layout.column: 0
                        enabled: model.traceButtonAvailability
                        onClicked: model.onTrace()
                        Layout.preferredWidth: clearBtn.width
                    }
                    Button {
                        id: clearBtn
                        text: "Clear Selected"
                        Layout.row: 1
                        Layout.column: 1
                        Layout.fillWidth: true
                        onClicked: model.onClear()
                        enabled: model.clearButtonAvailability
                    }
                    Text {
                        id: status
                        Layout.row: 2
                        Layout.columnSpan: 2
                        bottomPadding: 10
                        text: model.message
                    }
                    ProgressBar {
                        id: isBusy
                        visible: model.progressBarVisibility
                        Layout.row: 3
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        indeterminate: true
                    }
                }
            }
        }
    }
    Rectangle {
        id: updateWindow
        anchors.centerIn: parent
        visible: model.updateWindowVisibility
        enabled: visible
        width: childrenRect.width
        height: childrenRect.height
        color: "white"
        opacity: 1
        border.color: "black"
        border.width: 1
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
        }
        Column{
            padding: 10
            GridLayout {
                columns: 2
                rows: 2
                Text {
                    text: model.fieldName
                    Layout.alignment: Qt.AlignRight
                    rightPadding: 20
                }

                ComboBox {
                    id: comboBox
                    model: model.choices
                }

                Button {
                    text: "Update and Apply Edit"
                    onClicked: model.onApplyEdits(comboBox.currentText)
                }

                Button {
                    text: "Cancel"
                    onClicked: {
                        model.onClear();
                        comboBox.currentIndex = 0;
                    }
                }
            }
        }
    }
}
