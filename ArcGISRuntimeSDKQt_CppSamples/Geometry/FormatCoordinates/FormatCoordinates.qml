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
import QtQuick.Layouts 1.1
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

FormatCoordinatesSample {
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property int labelWidth: 200 * scaleFactor
    property int coordinateTextWidth: 200 * scaleFactor
    property int fontPixelSize: 14 * scaleFactor
    property int textPadding: 4 * scaleFactor
    property int textMargins: 10 * scaleFactor

    MapView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: leftColumnRect.top
        }

        objectName: "mapView"
    }

    Rectangle {
        id: leftColumnRect

        anchors {
            left: parent.left
            bottom: parent.bottom
            right: rightColRect.left
        }
        height: 160 * scaleFactor
        width: parent.width / 2

        Text {
            id: labelDD
            anchors.top: parent.top
            anchors.right: parent.right
            width: labelWidth
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: qsTr("Decimal Degrees: ")
        }

        Text {
            id: labelDMS
            anchors.top: labelDD.bottom
            anchors.right: parent.right
            width: labelWidth
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: qsTr("Degrees Minutes Seconds: ")
        }

        Text {
            id: labelUtm
            anchors.top: labelDMS.bottom
            anchors.right: parent.right
            width: labelWidth
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: qsTr("UTM: ")
        }

        Text {
            id: labelUsng
            anchors.top: labelUtm.bottom
            anchors.right: parent.right
            width: labelWidth
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: qsTr("USNG: ")
        }
    }

    Rectangle {
        id: rightColRect

        anchors {
            top: leftColumnRect.top
            left: leftColumnRect.right
            bottom: parent.bottom
            right: parent.right
        }
        height: leftColumnRect.height
        width: parent.width - leftColumnRect.width

        TextField {
            id: textDD
            anchors.top: parent.top
            anchors.left: parent.left
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            placeholderText: "nn.nnnN nnn.nnnW"
            text: coordinatesInDD
            onAccepted: {
                handleTextUpdate("Decimal Degrees", text);
            }
        }

        TextField {
            id: textDMS
            anchors.top: textDD.bottom
            anchors.left: parent.left
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            placeholderText: "nn nn nn.nN nnn nn nn.nW"
            text: coordinatesInDMS
            onAccepted: {
                handleTextUpdate("Degrees Minutes Seconds", text);
            }
        }

        TextField {
            id: textUtm
            anchors.top: textDMS.bottom
            anchors.left: parent.left
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            placeholderText: "nnS nnnnnn nnnnnnn"
            text: coordinatesInUtm
            onAccepted: {
                handleTextUpdate("UTM", text);
            }
        }

        TextField {
            id: textUsng
            anchors.top: textUtm.bottom
            anchors.left: parent.left
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            placeholderText: "nnS LC nnnn nnnn"
            text: coordinatesInUsng
            onAccepted: {
                handleTextUpdate("USNG", text);
            }
        }
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
}


