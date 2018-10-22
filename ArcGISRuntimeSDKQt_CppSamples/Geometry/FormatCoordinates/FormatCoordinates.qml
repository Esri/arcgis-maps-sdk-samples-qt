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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Esri.Samples 1.0

FormatCoordinatesSample {
    id: formatCoordinates
    clip: true
    width: 800
    height: 600

    property real scaleFactor: 1
    property int fontPixelSize: 14 * scaleFactor
    property int textPadding: Qt.platform.os === "android" ? (10 * scaleFactor) : (4 * scaleFactor)
    property string labelSuffix: ":  "

    MapView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: formatRect.top
        }

        objectName: "mapView"
    }

    Rectangle {
        id: formatRect
        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }
        height: childrenRect.height
        width: parent.width

        GridLayout {
            columns: 2
            width: parent.width
            Text {
                id: labelDD
                font.pixelSize: fontPixelSize
                padding: textPadding
                horizontalAlignment: Text.AlignRight
                text: strDecimalDegrees + labelSuffix
            }

            TextField {
                id: textDD
                font.pixelSize: fontPixelSize
                text: coordinatesInDD.length === 0 ? "invalid point" : coordinatesInDD
                Layout.fillWidth: true
                onAccepted: {
                    handleTextUpdate(strDecimalDegrees, text);
                }
            }

            Text {
                id: labelDMS
                font.pixelSize: fontPixelSize
                padding: textPadding
                horizontalAlignment: Text.AlignRight
                text: strDegreesMinutesSeconds + labelSuffix
            }

            TextField {
                id: textDMS
                font.pixelSize: fontPixelSize
                text: coordinatesInDMS.length === 0 ? "invalid point" : coordinatesInDMS
                Layout.fillWidth: true
                onAccepted: {
                    handleTextUpdate(strDegreesMinutesSeconds, text);
                }
            }

            Text {
                id: labelUtm
                font.pixelSize: fontPixelSize
                padding: textPadding
                horizontalAlignment: Text.AlignRight
                text: strUtm + labelSuffix
            }

            TextField {
                id: textUtm
                font.pixelSize: fontPixelSize
                text: coordinatesInUtm.length === 0 ? "invalid point" : coordinatesInUtm
                Layout.fillWidth: true
                onAccepted: {
                    handleTextUpdate(strUtm, text);
                }
            }

            Text {
                id: labelUsng
                font.pixelSize: fontPixelSize
                padding: textPadding
                horizontalAlignment: Text.AlignRight
                text: strUsng + labelSuffix
            }

            TextField {
                id: textUsng
                font.pixelSize: fontPixelSize
                text: coordinatesInUsng.length === 0 ? "invalid point" : coordinatesInUsng
                Layout.fillWidth: true
                onAccepted: {
                    handleTextUpdate(strUsng, text);
                }
            }
        }
    }
}
