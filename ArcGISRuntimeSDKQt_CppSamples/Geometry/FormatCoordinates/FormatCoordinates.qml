// [WriteFile Name=FormatCoordinates, Category=Geometry]
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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

FormatCoordinatesSample {
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property int labelWidth: 200 * scaleFactor
    property int coordinateTextWidth: 200 * scaleFactor

    MapView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: displayCoordinatesRect.top
        }

        objectName: "mapView"
    }

    Rectangle {
        id: displayCoordinatesRect
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
        }
        height: 120 * scaleFactor

        color: "lightgrey"
        border {
            color: "#4D4D4D"
            width: 1
        }

//        Column {
//            spacing: 10 * scaleFactor
//            anchors.fill: parent
//            anchors.margins: 10 * scaleFactor

            ListView {
                model: coordinateTextModel
                delegate: coordinateTextDelegate
            }
//        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }

    // ListModel and delegate for viewing coord text
    ListModel {
        id: coordinateTextModel
        ListElement {
            name: "Decimal Degrees"
            startText: "nn.nnnN nnn.nnnW"
        }
        ListElement {
            name: "Degrees Minutes Seconds"
            startText: "nn nn nn.nN nnn nn nn.nW"
        }
        ListElement {
            name: "UTM"
            startText: "nnS nnnnnn nnnnnnn"
        }
        ListElement {
            name: "USNG"
            startText: "nnS LC nnnn nnnn"
        }
    }

    Component {
        id: coordinateTextDelegate
        Row {
            spacing: 10 * scaleFactor
            Text {
                width: labelWidth
                text: name + ':'
                wrapMode: Text.WordWrap
                font.pixelSize: 14 * scaleFactor
            }

            TextField {
                width: coordinateTextWidth
                placeholderText: startText
            }
        }
    }

}


