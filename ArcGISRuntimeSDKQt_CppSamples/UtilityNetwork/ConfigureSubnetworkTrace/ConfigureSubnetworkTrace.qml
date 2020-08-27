// [WriteFile Name=ConfigureSubnetworkTrace, Category=UtilityNetwork]
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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12
import Esri.Samples 1.0

Item {
    readonly property var comparisonOperatorModel: ["Equal","NotEqual","GreaterThan","GreaterThanEqual","LessThan","LessThanEqual","IncludesTheValues","DoesNotIncludeTheValues","IncludesAny","DoesNotIncludeAny"]

    Rectangle {
        id: rootRectangle
        anchors.fill: parent
        color: "lightgrey"

        ScrollView {
            id: scrollView
            anchors.fill: parent

            ColumnLayout {
                id: controlItemsLayout
                anchors.left: parent.left
                anchors.right: parent.right

                CheckBox {
                    text: qsTr("Include barriers")
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                    checkState: Qt.Checked
                    onCheckStateChanged: model.changeIncludeBarriersState(checked);
                }

                CheckBox {
                    text: qsTr("Include containers")
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                    checkState: Qt.Checked
                    onCheckStateChanged: model.changeIncludeContainersState(checked);
                }

                Shape {
                    height: 2
                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"
                        strokeStyle: ShapePath.SolidLine
                        startX: 2; startY: 0
                        PathLine { x: controlItemsLayout.width - 2 ; y: 0 }
                    }
                }

                Text {
                    text: qsTr("Example barrier condition for this data. 'Transformer Load' Equal '15'")
                    font.pixelSize: 11
                    Layout.minimumWidth: rootRectangle.width
                    horizontalAlignment: Text.AlignHCenter
                    enabled: !model.busy
                }

                Shape {
                    height: 2
                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"
                        strokeStyle: ShapePath.SolidLine
                        startX: 2; startY: 0
                        PathLine { x: controlItemsLayout.width - 2 ; y: 0 }
                    }
                }

                Text {
                    text: qsTr("New Barrier Condition:")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: networkAttributeComboBox
                    model: model.attributeListModel ? model.attributeListModel : null
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible

                    onModelChanged: currentIndex = 0;

                    onCurrentTextChanged: model.codedValueOrInputText(currentText);
                }

                ComboBox {
                    id: comparisonOperatorComboBox
                    model: comparisonOperatorModel
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                }

                RowLayout {
                    enabled: !model.busy
                    ComboBox {
                        id: valueSelectionComboBox
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        visible: !model.textFieldVisible
                        model: model.valueSelectionListModel
                    }

                    TextField {
                        id: inputTextField
                        Layout.minimumHeight: valueSelectionComboBox.height
                        Layout.fillWidth: true
                        visible: model.textFieldVisible
                        validator: DoubleValidator {}
                        color: "black"
                        placeholderText: qsTr("Enter value here")
                        placeholderTextColor: "black"
                        selectByMouse: true
                        background: Rectangle {
                            anchors.centerIn: parent
                            height: parent.height
                            width: parent.width
                            color: "white"
                        }
                    }
                }

                Button {
                    text: qsTr("Add")
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                    onClicked: {
                        if (model.textFieldVisible) {
                            if (inputTextField.text)
                                model.addCondition(networkAttributeComboBox.currentText, comparisonOperatorComboBox.currentIndex, inputTextField.text);
                            return;
                        } else {
                            model.addCondition(networkAttributeComboBox.currentText, comparisonOperatorComboBox.currentIndex, valueSelectionComboBox.currentIndex);
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.maximumWidth: rootRectangle.width
                    Layout.minimumHeight: 50
                    Layout.maximumHeight: .15 * rootRectangle.height
                    clip: true
                    Row {
                        anchors.fill: parent
                        Text {
                            text: model.expressionBuilder
                        }
                    }
                }

                RowLayout {
                    enabled: !model.busy
                    Button {
                        text: qsTr("Trace")
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        onClicked: model.trace();
                    }

                    Button {
                        text: qsTr("Reset")
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        onClicked: model.reset();
                    }
                }
            }
        }
    }

    Dialog {
        id: dialog
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Text {
            text: model.dialogText
            anchors.centerIn: parent
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: model.busy
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ConfigureSubnetworkTraceSample {
        id: model

        onShowDialog: dialog.open();
    }
}
