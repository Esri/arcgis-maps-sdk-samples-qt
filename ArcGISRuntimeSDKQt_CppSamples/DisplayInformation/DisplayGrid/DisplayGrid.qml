// [WriteFile Name=DisplayGrid, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import Esri.Samples 1.0

DisplayGridSample {
    id: displayGrid
    clip: true
    width: 800
    height: 600
    
    property real centerWindowY: (displayGrid.height / 2) - (styleWindow.height / 2)

    MapView {
        id: mapQuickView
        anchors.fill: parent
    }
    mapView: mapQuickView

    // Button to view the styling window
    Rectangle {
        id: styleButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }

        width: 150
        height: 30
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 8
        border {
            color: "#585858"
            width: 1
        }

        Text {
            anchors.centerIn: parent
            text: "Change grid style"
            font.pixelSize: 14
            color: "#474747"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: styleButton.pressed = true
            onReleased: styleButton.pressed = false
            onClicked: {
                background.visible = true;
                showAnimation.restart()
            }
        }
    }

    // background fade rectangle
    Rectangle {
        id: background
        anchors.fill: parent
        color: "gray"
        opacity: 0.7
        visible: false
        border {
            width: 0.5
            color: "black"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
        }
    }

    Rectangle {
        id: styleWindow
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height // initially display below the page
        height: childrenRect.height
        width: childrenRect.width

        color: "lightgray"
        radius: 4
        border {
            color: "black"
            width: 1
        }

        SequentialAnimation {
            id: showAnimation
            NumberAnimation { target: styleWindow; property: "y"; to: centerWindowY; duration: 200 }
        }

        SequentialAnimation {
            id: hideAnimation
            NumberAnimation { target: styleWindow; property: "y"; to: displayGrid.height; duration: 200 }
        }

        GridLayout {
            id: grid
            columns: 2

            Text {
                Layout.leftMargin: 10
                text: "Grid type"
            }

            ComboBox {
                id: gridTypeComboBox
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [latlonGrid, mgrsGrid, utmGrid, usngGrid]
                onCurrentTextChanged: changeGrid(currentText)
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metricsGridTypeComboBox.text = model[i];
                        modelWidth = Math.max(modelWidth, metricsGridTypeComboBox.width);
                    }
                }
                TextMetrics {
                    id: metricsGridTypeComboBox
                    font: gridTypeComboBox.font
                }
            }

            Text {
                text: "Labels visible"
                Layout.leftMargin: 10
                enabled: gridVisibleSwitch.checked
                color: enabled ? "black" : "gray"
            }

            Switch {
                id: labelVisibleSwitch
                Layout.rightMargin: 10
                checked: true
                enabled: gridVisibleSwitch.checked
                onCheckedChanged: gridLabelVisibility = checked
            }

            Text {
                Layout.leftMargin: 10
                text: "Grid visible"
            }

            Switch {
                id: gridVisibleSwitch
                Layout.rightMargin: 10
                checked: true
                onCheckedChanged: gridVisibility = checked
            }

            Text {
                Layout.leftMargin: 10
                text: "Grid color"
            }

            ComboBox {
                id: colorCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: ["red", "white", "blue"]
                onCurrentTextChanged: currentGridColor = currentText
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        colorComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: colorComboMetrics
                    font: colorCombo.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label color"
            }

            ComboBox {
                id: colorCombo2
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: ["red", "black", "blue"]
                onCurrentTextChanged: currentLabelColor = currentText;
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        colorCombo2Metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorCombo2Metrics.width);
                    }
                }
                TextMetrics {
                    id: colorCombo2Metrics
                    font: colorCombo2.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label position"
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                id: positionCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [geographicPosition, bottomLeftPosition, bottomRightPosition, topLeftPosition, topRightPosition, centerPosition, allSidesPosition]
                onCurrentTextChanged: currentLabelPosition = currentText;
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        positionComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, positionComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: positionComboMetrics
                    font: positionCombo.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label format"
                enabled: gridTypeComboBox.currentText == latlonGrid
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                id: formatCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [ddFormat, dmsFormat]
                enabled: gridTypeComboBox.currentText == latlonGrid
                onCurrentTextChanged: currentLabelFormat = currentText;
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        formatComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, formatComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: formatComboMetrics
                    font: formatCombo.font
                }
            }

            // Button to hide the styling window
            Rectangle {
                id: hideButton
                property bool pressed: false
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.bottomMargin: 5

                width: 150
                height: 30
                color: pressed ? "#959595" : "#D6D6D6"
                radius: 8
                border {
                    color: "#585858"
                    width: 1
                }

                Text {
                    anchors.centerIn: parent
                    text: "Hide window"
                    font.pixelSize: 14
                    color: "#474747"
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: hideButton.pressed = true
                    onReleased: hideButton.pressed = false
                    onClicked: {
                        background.visible = false;
                        hideAnimation.restart()
                    }
                }
            }
        }
    }
}
