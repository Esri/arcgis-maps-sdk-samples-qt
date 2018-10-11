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
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import Esri.Samples 1.0

FormatCoordinatesSample {
    id: formatCoordinates
    clip: true
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property int labelWidth: 100 * scaleFactor
    property int coordinateTextWidth: 200 * scaleFactor
    property int fontPixelSize: 14 * scaleFactor
    property int textPadding: Qt.platform.os === "android" ? (10 * scaleFactor) : (4 * scaleFactor)
    property string labelSuffix: ":  "

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
        height: 140 * scaleFactor
        width: labelWidth

        Text {
            id: labelDD
            anchors {
                top: parent.top
                right: parent.right
            }
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: strDecimalDegrees + labelSuffix
        }

        Text {
            id: labelDMS
            anchors {
                top: labelDD.bottom
                right: parent.right
            }
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: strDegreesMinutesSeconds + labelSuffix
        }

        Text {
            id: labelUtm
            anchors {
                top: labelDMS.bottom
                right: parent.right
            }
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: strUtm + labelSuffix
        }

        Text {
            id: labelUsng
            anchors {
                top: labelUtm.bottom
                right: parent.right
            }
            font.pixelSize: fontPixelSize
            padding: textPadding
            horizontalAlignment: Text.AlignRight
            text: strUsng + labelSuffix
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
        width: coordinateTextWidth + (10 * scaleFactor)

        TextField {
            id: textDD
            anchors {
                top: parent.top
                left: parent.left
            }
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            text: coordinatesInDD.length === 0 ? "invalid point" : coordinatesInDD
            onAccepted: {
                handleTextUpdate(strDecimalDegrees, text);
            }
        }

        TextField {
            id: textDMS
            anchors {
                top: textDD.bottom
                left: parent.left
            }
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            text: coordinatesInDMS.length === 0 ? "invalid point" : coordinatesInDMS
            onAccepted: {
                handleTextUpdate(strDegreesMinutesSeconds, text);
            }
        }

        TextField {
            id: textUtm
            anchors {
                top: textDMS.bottom
                left: parent.left
            }
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            text: coordinatesInUtm.length === 0 ? "invalid point" : coordinatesInUtm
            onAccepted: {
                handleTextUpdate(strUtm, text);
            }
        }

        TextField {
            id: textUsng
            anchors {
                top: textUtm.bottom
                left: parent.left
            }
            width: coordinateTextWidth
            font.pixelSize: fontPixelSize
            text: coordinatesInUsng.length === 0 ? "invalid point" : coordinatesInUsng
            onAccepted: {
                handleTextUpdate(strUsng, text);
            }
        }
    }
}
