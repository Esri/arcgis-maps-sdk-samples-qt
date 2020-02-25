// [WriteFile Name=ConfigureSubnetworkTrace, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.

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

        ColumnLayout {
            id: controlItemsLayout
            anchors.left: parent.left
            anchors.right: parent.right
            Layout.alignment: Qt.AlignHCenter
            CheckBox {
                text: qsTr("Include barriers")
                Layout.fillWidth: true
                enabled: !model.busy
//                Layout.alignment: Qt.AlignRight
                checkState: Qt.Checked
                onCheckStateChanged: model.changeIncludeBarriersState(checked);
            }

            CheckBox {
                text: qsTr("Include containers")
                Layout.fillWidth: true
                enabled: !model.busy
//                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            ComboBox {
                id: networkAttributeComboBox
                model: model.attributeListModel ? model.attributeListModel : null
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                enabled: !model.busy

                onModelChanged: currentIndex = 0;

                onCurrentTextChanged: model.codedValueOrInputText(currentText);
            }

            ComboBox {
                id: comparisonOperatorComboBox
                model: comparisonOperatorModel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                enabled: !model.busy
            }

            RowLayout {
                enabled: !model.busy
                ComboBox {
                    id: valueSelectionComboBox
                    Layout.fillWidth: true
                    visible: !model.textFieldVisible
                    model: model.valueSelectionListModel
                }

                TextField {
                    id: inputTextField
                    Layout.minimumWidth: rootRectangle.width
                    visible: model.textFieldVisible
                    color: "black"
                    placeholderText: qsTr("Enter value here")
                    placeholderTextColor: "black"
                    background: Rectangle {
                        anchors.centerIn: parent
                        width: parent.width
                        color: "white"
                    }
                }
            }

            Button {
                text: qsTr("Add")
                Layout.fillWidth: true
                enabled: !model.busy
                onClicked: {
                    if (model.textFieldVisible) {
                        model.addCondition(networkAttributeComboBox.currentText, comparisonOperatorComboBox.currentIndex, inputTextField.text);
                    } else {
                        model.addCondition(networkAttributeComboBox.currentText, comparisonOperatorComboBox.currentIndex, valueSelectionComboBox.currentIndex);
                    }
                }
            }

            //            Rectangle {
            //                id: borderBoxForListView
            //                Layout.fillWidth: true
            //                Layout.minimumHeight: 150
            //                color: "transparent"
            //                Layout.margins: 2
            //                border {
            //                    color: "black"
            //                    width:  1
            //                }

            ScrollView {
                Layout.fillWidth: true
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



//            ListView {
//                id: groupingView
//                Layout.fillWidth: true
//                Layout.minimumHeight: 150
//                //                    Layout.fillHeight: true
//                //                    Layout.fillWidth: true

//                //                anchors {
//                //                    fill: parent
//                //                    margins: 5
//                //                }
//                //                highlight: highlightRectangle
//                //                highlightResizeVelocity: 1000000
//                //                highlightMoveVelocity: 1000000

//                clip: true
//                model: model.conditionBarrierExpressionListModel
//                delegate: Item {
//                    width: parent.width
//                    height: 25

//                    Row {
//                        anchors.verticalCenter: parent.verticalCenter
//                        spacing: 10
//                        Text {
//                            text: "(%1)".arg(condition)
//                            font.pixelSize: 12
//                        }
//                    }

//                    MouseArea {
//                        anchors.fill: parent
//                        onClicked: groupingView.currentIndex = index;
//                    }
//                }

//                onCurrentIndexChanged: {
//                    print("currentIndex: %1".arg(currentIndex));
//                }
//            }

            //            }

            RowLayout {
                enabled: !model.busy
                Button {
                    text: qsTr("Trace")
                    Layout.fillWidth: true
                    onClicked: model.trace();
                }

                Button {
                    text: qsTr("Reset")
                    Layout.fillWidth: true
                    onClicked: model.reset();
                }
            }

//            Button {
//                id: testButton
//                text: qsTr("test input text field to comboBox")
////                onClicked: inputTextField.visible = !inputTextField.visible
//                onClicked: {
//                }
//            }
        }
    }

    Dialog {
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: model.dialogVisible

        Text {
            text: model.dialogText
            anchors.centerIn: parent
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: model.busy
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ConfigureSubnetworkTraceSample {
        id: model
//        mapView: view
    }
}
