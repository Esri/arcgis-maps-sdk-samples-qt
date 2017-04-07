// [WriteFile Name=IdentifyGraphics, Category=DisplayInformation]
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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.1
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property int labelWidth: 200 * scaleFactor
    property int coordinateTextWidth: 200 * scaleFactor
    property int fontPixelSize: 14 * scaleFactor
    property int textPadding: 4 * scaleFactor

    MapView {
        id: mapView

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: leftColumnRect.top
        }

        Map {
            id: map
            // set the basemap
            BasemapImagery {}
        }

        // Add a graphics overlay to the map view
        GraphicsOverlay {
            id: graphicsOverlay
            // assign a render to the graphics overlay
            renderer: SimpleRenderer {
                symbol: SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleX
                    color: Qt.rgba(1.0, 0.0, 0.0, 1.0)
                    size: 15.0
                }
            }

            Graphic {
                id: locationGraphic
                geometry: Point {
                    x: -117.195723
                    y: 34.056195
                    spatialReference: SpatialReference {
                        wkid: 4326
                    }
                }
            }
        }

        onMouseClicked: {  // on MapView
            handleTextUpdate(mouse.mapPoint);
        }
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
            text: qsTr("Decimal Degrees:  ")
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
            text: qsTr("Degrees Minutes Seconds:  ")
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
            text: qsTr("UTM:  ")
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
            text: qsTr("USNG:  ")
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
            onAccepted: {
                handleTextUpdate("Decimal Degrees", text);
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
            onAccepted: {
                handleTextUpdate("Degrees Minutes Seconds", text);
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
            onAccepted: {
                handleTextUpdate("UTM", text);
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
            onAccepted: {
                handleTextUpdate("USNG", text);
            }
        }
    }

    function handleLocationUpdate(point) {
        locationGraphic.geometry = point;
        setTextFromPoint(point);
    }

    function handleTextUpdate(textType, text) {
        handleLocationUpdate(createPointFromText(textType, text));
    }

    function setTextFromPoint(point) {
        if (point.isEmpty) {
            return;
        }
        textDD.text   = CoordinateFormatter.toLatitudeLongitude(point, Enums.LatitudeLongitudeFormatDecimalDegrees, 6);
        textDMS.text  = CoordinateFormatter.toLatitudeLongitude(point, Enums.LatitudeLongitudeFormatDegreesMinutesSeconds, 1);
        textUsng.text = CoordinateFormatter.toUsng(point, 5, true);
        textUtm.text  = CoordinateFormatter.toUtm(point, Enums.UtmConversionModeLatitudeBandIndicators, true);
    }

    function createPointFromText(textType, text) {
        if ("Decimal Degrees" === textType
                || "Degrees Minutes Seconds" === textType) {
            return CoordinateFormatter.fromLatitudeLongitude(text, map.spatialReference);
        }
        if ("USNG" === textType) {
            return CoordinateFormatter.fromUsng(text, map.spatialReference);
        }
        if ("UTM" === textType) {
            return CoordinateFormatter.fromUtm(text, map.spatialReference);
        }
    }
}
