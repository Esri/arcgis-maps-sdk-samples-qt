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

    Rectangle {
        anchors.fill: parent
        color: "white"

        ColumnLayout {
            id: controlItemsLayout
            anchors.left: parent.left
            anchors.right: parent.right
            CheckBox {
                text: qsTr("Include barriers:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            CheckBox {
                text: qsTr("Include containers:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
                //                model: ["One", "Two", "Three"] // test model
                id: networkAttributeComboBox
                model: comparisonSourceListModel ? comparisonSourceListModel : null
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                textRole: comparisonSourceListModel ? "name" : null

                onModelChanged: currentIndex = 0;

                onCurrentTextChanged: {
                    if(networkDefinition) {
                        let workingDomain = networkDefinition.networkAttribute(currentText).domain;
                        if (workingDomain) {
                            print("domain: %1".arg(workingDomain.domainType)); //* remove
                            print(workingDomain.codedValues.length); //* remove
                            valueSelectionListModel.clear();
                            for(let i = 0; i < workingDomain.codedValues.length; i++) {
                                print(workingDomain.codedValues[i].name + " - " + workingDomain.codedValues[i].code); //* remove
                                valueSelectionListModel.append({value: workingDomain.codedValues[i].name})

                            }
                            valueSelectionComboBox.model = valueSelectionListModel;
                            valueSelectionComboBox.currentIndex = 0;

                            inputTextField.visible = false;


                        } else {
                            inputTextField.visible = true;
                        }



                        print(networkDefinition.networkAttribute(currentText).name);
                    }
                }
            }

            ComboBox {
                //                model: ["Four", "TFive", "Six"] // test model
                id: comparisonOperatorComboBox
                model: comparisonOperatorModel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            RowLayout {
                ComboBox {
                    //                    model: ["One", "Two", "Three"]  // test model
                    id: valueSelectionComboBox
                    Layout.fillWidth: true
                    visible: !inputTextField.visible

//                    onModelChanged: currentIndex = 0;
//                    onVisibleChanged: {
//                        if (visible)
//                            currentIndex = 0;
//                    }
                }

                TextField {
                    id: inputTextField
                    Layout.fillWidth: true
                    visible: true
                }
            }

            Button {
                text: qsTr("Add")
                Layout.fillWidth: true
                onClicked: {
//                    myModel.append({condition: "`Operation Device Status` Equal `Open` %1".arg(inc++)});
                    addCondition();
                    print("appended");
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

            ListView {
                id: groupingView
                Layout.fillWidth: true
                Layout.minimumHeight: 150
                //                    Layout.fillHeight: true
                //                    Layout.fillWidth: true

                //                anchors {
                //                    fill: parent
                //                    margins: 5
                //                }
                //                highlight: highlightRectangle
                //                highlightResizeVelocity: 1000000
                //                highlightMoveVelocity: 1000000

                clip: true
                model: myModel
                delegate: Item {
                    width: parent.width
                    height: 25

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 10
                        Text {
                            text: "(%1)".arg(condition)
                            font.pixelSize: 12
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: groupingView.currentIndex = index;
                    }
                }

                onCurrentIndexChanged: {
                    print("currentIndex: %1".arg(currentIndex));
                }
            }

            //            }

            RowLayout {
                Button {
                    text: qsTr("Trace")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Reset")
                    Layout.fillWidth: true
                }
            }

            Button {
                id: testButton
                text: qsTr("test input text field to comboBox")
//                onClicked: inputTextField.visible = !inputTextField.visible
                onClicked: {
                    let workingNetworkAttribute = networkDefinition.networkAttribute(networkAttributeComboBox.currentText);
                    print(workingNetworkAttribute.domain.codedValues[valueSelectionComboBox.currentIndex].code);
                    print(valueSelectionComboBox.currentIndex);
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ConfigureSubnetworkTraceSample {
        id: model
//        mapView: view
    }
}
