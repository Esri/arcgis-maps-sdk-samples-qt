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

Item {
    clip: true

    property int fontPixelSize: 14
    property string labelSuffix: ":  "

    FormatCoordinatesSample {
        id: model
        mapView: view
    }

    Rectangle {
        anchors.fill: parent

        GridLayout {
            columns: 2
            anchors.fill: parent

            MapView {
                id: view
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.columnSpan: 2
            }

            Text {
                id: labelDD
                font.pixelSize: fontPixelSize
                Layout.margins: 5
                horizontalAlignment: Text.AlignRight
                text: model.strDecimalDegrees + labelSuffix
            }

            TextField {
                id: textDD
                font.pixelSize: fontPixelSize
                text: model.coordinatesInDD.length === 0 ? "invalid point" : model.coordinatesInDD
                Layout.fillWidth: true
                Layout.margins: 5
                onAccepted: {
                    model.handleTextUpdate(model.strDecimalDegrees, text);
                }
            }

            Text {
                id: labelDMS
                font.pixelSize: fontPixelSize
                Layout.margins: 5
                horizontalAlignment: Text.AlignRight
                text: model.strDegreesMinutesSeconds + labelSuffix
            }

            TextField {
                id: textDMS
                font.pixelSize: fontPixelSize
                Layout.margins: 5
                text: model.coordinatesInDMS.length === 0 ? "invalid point" : model.coordinatesInDMS
                Layout.fillWidth: true
                onAccepted: {
                    model.handleTextUpdate(model.strDegreesMinutesSeconds, text);
                }
            }

            Text {
                id: labelUtm
                font.pixelSize: fontPixelSize
                Layout.margins: 5
                horizontalAlignment: Text.AlignRight
                text: model.strUtm + labelSuffix
            }

            TextField {
                id: textUtm
                font.pixelSize: fontPixelSize
                text: model.coordinatesInUtm.length === 0 ? "invalid point" : model.coordinatesInUtm
                Layout.fillWidth: true
                Layout.margins: 5
                onAccepted: {
                    model.handleTextUpdate(model.strUtm, text);
                }
            }

            Text {
                id: labelUsng
                font.pixelSize: fontPixelSize
                Layout.margins: 5
                horizontalAlignment: Text.AlignRight
                text: model.strUsng + labelSuffix
            }

            TextField {
                id: textUsng
                font.pixelSize: fontPixelSize
                text: model.coordinatesInUsng.length === 0 ? "invalid point" : model.coordinatesInUsng
                Layout.fillWidth: true
                Layout.margins: 5
                onAccepted: {
                    model.handleTextUpdate(model.strUsng, text);
                }
            }
        }
    }
}
