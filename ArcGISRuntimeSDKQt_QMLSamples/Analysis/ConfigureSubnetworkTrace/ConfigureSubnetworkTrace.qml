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
import Esri.ArcGISRuntime 100.8

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property alias myModel: myModel
    property int inc: 0

    ListModel {
        id: myModel
    }

    Rectangle {
        anchors.fill: parent
        color: "white"

        ColumnLayout {
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

            Text {
                text: qsTr("Example barrier condition for this data. 'Transformer Load' Equal '15'")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            Text {
                text: qsTr("New Barrier Condition:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            ComboBox {
//                model: ["One", "Two", "Three"] // test model
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            ComboBox {
//                model: ["Four", "TFive", "Six"] // test model
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            RowLayout {
                ComboBox {
                    id: attributes
//                    model: ["One", "Two", "Three"]  // test model
                    Layout.fillWidth: true
                    visible: !inputTextField.visible
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
                    myModel.append({condition: "`Operation Device Status` Equal `Open` %1".arg(inc++)});
                }
            }

            ListView {
                id: groupingView
                Layout.fillWidth: true
                Layout.minimumHeight: 150

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
                onClicked: inputTextField.visible = !inputTextField.visible
            }
        }
    }

    //    MapView {
    //        id: mapView
    //        anchors.fill: parent

    //        Map {
    //            BasemapTopographic {}
    //        }
    //    }
}
