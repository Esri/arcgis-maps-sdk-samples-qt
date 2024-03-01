// [WriteFile Name=ConfigureBasemapStyleLanguage, Category=Maps]
// [Legal]
// Copyright 2024 Esri.

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

    Rectangle {
        id: rectangle
        anchors {
            right: parent.right
            top: parent.top
            margins: 15
        }
        width: column.width
        height: column.height
        color: Qt.rgba(0, 0, 0, 0.5)
        border {
            color: "black"
            width: 1
        }
        opacity: 0.9

        Column {
            id: column
            padding: 10
            spacing: 5
            width: 300

            Column {
                id:languageStrategy
                spacing: 5

                Text {
                    text: "Set Language Strategy:"
                    font.bold: true
                    color: "white"
                }

                Row {
                    RadioButton {
                        id: globalButton
                        onCheckedChanged: {
                            setNewBasemap();
                            localButton.checked = !globalButton.checked
                        }
                        enabled: comboBox.currentText == "none"
                    }

                    Text {
                        id: globalText
                        text: "Global"
                        height: globalButton.height
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }
                }

                Row {
                    RadioButton {
                        id: localButton
                        checked: true
                        onCheckedChanged: {
                            setNewBasemap();
                            globalButton.checked = !localButton.checked
                        }
                        enabled: comboBox.currentText == "none"
                    }
                    Text {
                        id: localText
                        text: "Local"
                        height: localButton.height
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }
                }

            }

            Column {
                id:specificLanguage

                Text {
                    text: "Set Specific Language:"
                    font.bold: true
                    color: "white"
                }

                ComboBox {
                    id: comboBox
                    model: ["none" , "Bulgarian", "Greek", "Turkish"]
                    onCurrentTextChanged: setNewBasemap()
                }
            }
        }
    }

    function setNewBasemap() {
        model.setNewBasemap(globalButton.checked , comboBox.currentText);
    }
}
