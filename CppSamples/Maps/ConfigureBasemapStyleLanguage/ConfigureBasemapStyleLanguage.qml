// [WriteFile Name=ConfigureBasemapStyleLanguage, Category=Maps]
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
import Esri.Samples

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

    // Declare the C++ instance which creates the map etc. and supply the view
    ConfigureBasemapStyleLanguageSample {
        id: model
        mapView: view
    }

    Pane {
        id: rectangle
        anchors {
            right: parent.right
            top: parent.top
            margins: 15
        }
        width: column.width
        height: column.height
        opacity: 0.9

        Column {
            id: column
            padding: 10
            spacing: 5
            width: 300
            anchors.centerIn: parent

            Column {
                id:languageStrategy
                spacing: 5

                Label {
                    text: qsTr("Set Language Strategy:")
                    font.bold: true
                }

                Row {
                    RadioButton {
                        id: globalButton
                        onCheckedChanged: {
                            model.setNewBasemapLanguage(globalButton.checked, comboBox.currentText);
                            localButton.checked = !globalButton.checked
                        }
                        enabled: comboBox.currentText == "none"
                    }

                    Label {
                        id: globalText
                        text: qsTr("Global")
                        height: globalButton.height
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Row {
                    RadioButton {
                        id: localButton
                        checked: true
                        onCheckedChanged: {
                            model.setNewBasemapLanguage(globalButton.checked, comboBox.currentText);
                            globalButton.checked = !localButton.checked
                        }
                        enabled: comboBox.currentText == "none"
                    }
                    Label {
                        id: localText
                        text: qsTr("Local")
                        height: localButton.height
                        verticalAlignment: Text.AlignVCenter
                    }
                }

            }

            Column {
                id:specificLanguage

                Label {
                    text: qsTr("Set Specific Language:")
                    font.bold: true
                }

                ComboBox {
                    id: comboBox
                    model: [qsTr("none") , qsTr("Bulgarian"), qsTr("Greek"), qsTr("Turkish")]
                    // onCurrentTextChanged executes when the text is changed and also initially on loading the sample
                    onCurrentTextChanged: model.setNewBasemapLanguage(globalButton.checked, comboBox.currentText);
                }
            }
        }
    }
}
