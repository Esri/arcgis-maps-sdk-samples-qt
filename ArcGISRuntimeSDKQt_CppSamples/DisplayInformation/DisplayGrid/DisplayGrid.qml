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
import QtQuick.Window 2.2
import Esri.Samples 1.0

DisplayGridSample {
    id: displayGrid
    clip: true
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property real centerWindowY: (displayGrid.height / 2) - (styleWindow.height / 2)

    mapView: MapView {
        id: mapQuickView
        anchors.fill: parent
    }

    // Button to view the styling window
    Rectangle {
        id: styleButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23 * scaleFactor
        }

        width: 150 * scaleFactor
        height: 30 * scaleFactor
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 8
        border {
            color: "#585858"
            width: 1 * scaleFactor
        }

        Text {
            anchors.centerIn: parent
            text: "Change grid style"
            font.pixelSize: 14 * scaleFactor
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
            width: 0.5 * scaleFactor
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
        width: 250 * scaleFactor
        height: childrenRect.height

        color: "lightgray"
        radius: 4 * scaleFactor
        border {
            color: "black"
            width: 1 * scaleFactor
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
            anchors {
                left: parent.left;
                right: parent.right
                margins: 10 * scaleFactor;
            }
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Text {
                text: "Grid type"
            }

            ComboBox {
                id: gridTypeComboBox
                model: [latlonGrid, mgrsGrid, utmGrid, usngGrid]
                onCurrentTextChanged: changeGrid(currentText)
            }

            Text {
                text: "Labels visible"
                enabled: gridVisibleSwitch.checked
                color: enabled ? "black" : "gray"
            }

            Switch {
                id: labelVisibleSwitch
                checked: true
                enabled: gridVisibleSwitch.checked
                onCheckedChanged: gridLabelVisibility = checked
            }

            Text {
                text: "Grid visible"
            }

            Switch {
                id: gridVisibleSwitch
                checked: true
                onCheckedChanged: gridVisibility = checked
            }

            Text {
                text: "Grid color"
            }

            ComboBox {
                model: ["red", "white", "blue"]
                onCurrentTextChanged: currentGridColor = currentText
            }

            Text {
                text: "Label color"
            }

            ComboBox {
                model: ["red", "black", "blue"]
                onCurrentTextChanged: currentLabelColor = currentText;
            }

            Text {
                text: "Label position"
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                model: [geographicPosition, bottomLeftPosition, bottomRightPosition, topLeftPosition, topRightPosition, centerPosition, allSidesPosition]
                onCurrentTextChanged: currentLabelPosition = currentText;
            }

            Text {
                text: "Label format"
                enabled: gridTypeComboBox.currentText == latlonGrid
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                model: [ddFormat, dmsFormat]
                enabled: gridTypeComboBox.currentText == latlonGrid
                onCurrentTextChanged: currentLabelFormat = currentText;
            }

            // Button to hide the styling window
            Rectangle {
                id: hideButton
                property bool pressed: false
                anchors.horizontalCenter: parent.horizontalCenter
                Layout.columnSpan: 2

                width: 150 * scaleFactor
                height: 30 * scaleFactor
                color: pressed ? "#959595" : "#D6D6D6"
                radius: 8
                border {
                    color: "#585858"
                    width: 1 * scaleFactor
                }

                Text {
                    anchors.centerIn: parent
                    text: "Hide window"
                    font.pixelSize: 14 * scaleFactor
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
