// [WriteFile Name=ViewshedLocation, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

ViewshedLocationSample {
    id: viewshedSample
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    Rectangle {
        id: instructionRect
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }
        width: 150 * scaleFactor
        height: 50 * scaleFactor
        radius: 4 * scaleFactor
        border {
            width: 1 * scaleFactor
            color: "black"
        }
        color: "lightgray"
        opacity: 0.85

        Text {
            anchors.centerIn: parent
            text: "Press, Hold, and Drag\nto perform Viewshed"
        }
    }

    Column {
        anchors {
            top: instructionRect.bottom
            left: parent.left
            bottom: parent.bottom
            margins: 8 * scaleFactor
        }
        spacing: 20 * scaleFactor
        width: 175 * scaleFactor

        Row {
            width: parent.width
            height: 25 * scaleFactor
            spacing: 5 * scaleFactor

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.75
                text: qsTr("Viewshed Visible")
                font.pixelSize: 14 * scaleFactor
            }

            Switch {
                anchors.verticalCenter: parent.verticalCenter
                width: 150 * scaleFactor
                checked: true
                onCheckedChanged: viewshedSample.viewshedVisible = checked;
            }
        }

        ViewshedSlider {
            titleText: qsTr("Min Distance (m)")
            parameterValue: viewshedSample.minDistance
            minValue: 1
            maxValue: 2000
            onParameterValueChanged: viewshedSample.minDistance = parameterValue;
        }

        ViewshedSlider {
            titleText: qsTr("Max Distance (m)")
            parameterValue: viewshedSample.maxDistance
            minValue: 1
            maxValue: 2000
            onParameterValueChanged: viewshedSample.maxDistance = parameterValue;
        }

        ViewshedSlider {
            titleText: qsTr("Vertical Angle")
            parameterValue: viewshedSample.verticalAngle
            minValue: 1
            maxValue: 179
            onParameterValueChanged: viewshedSample.verticalAngle = parameterValue;
        }

        ViewshedSlider {
            titleText: qsTr("Horizontal Angle")
            parameterValue: viewshedSample.horizontalAngle
            minValue: 1
            maxValue: 179
            onParameterValueChanged: viewshedSample.horizontalAngle = parameterValue;
        }

        ViewshedSlider {
            titleText: qsTr("Heading")
            parameterValue: viewshedSample.heading
            minValue: 1
            maxValue: 359
            onParameterValueChanged: viewshedSample.heading = parameterValue;
        }

        ViewshedSlider {
            titleText: qsTr("Pitch")
            parameterValue: viewshedSample.pitch
            minValue: 1
            maxValue: 179
            onParameterValueChanged: viewshedSample.pitch = parameterValue;
        }

        Row {
            width: parent.width
            height: 25 * scaleFactor
            spacing: 5 * scaleFactor

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.85
                text: qsTr("Visible Color")
                font.pixelSize: 14 * scaleFactor
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 25 * scaleFactor
                height: width
                radius: 4 * scaleFactor
                border {
                    color: "black"
                    width: 1 * scaleFactor
                }
                color: viewshedSample.visibleColor

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        visibleColorDialog.open();
                    }
                }
            }
        }

        Row {
            width: parent.width
            height: 25 * scaleFactor
            spacing: 5 * scaleFactor

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.85
                text: qsTr("Obstructed Color")
                font.pixelSize: 14 * scaleFactor
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 25 * scaleFactor
                height: width
                radius: 4 * scaleFactor
                border {
                    color: "black"
                    width: 1 * scaleFactor
                }
                color: viewshedSample.obstructedColor

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        obstructedColorDialog.open();
                    }
                }
            }
        }
    }

    ColorDialog {
        id: visibleColorDialog
        onAccepted: {
            close();
            viewshedSample.visibleColor = color;
        }
    }

    ColorDialog {
        id: obstructedColorDialog
        onAccepted: {
            close();
            viewshedSample.obstructedColor = color;
        }
    }
}
