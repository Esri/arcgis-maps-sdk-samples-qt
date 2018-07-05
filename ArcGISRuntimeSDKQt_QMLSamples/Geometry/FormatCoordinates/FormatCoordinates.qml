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
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property int labelWidth: 100 * scaleFactor
    property int coordinateTextWidth: 200 * scaleFactor
    property int fontPixelSize: 14 * scaleFactor
    property int textPadding: Qt.platform.os === "android" ? (10 * scaleFactor) : (4 * scaleFactor)
    property string labelSuffix: ":  "

    property string strDecimalDegrees: qsTr("Degrees")
    property string strDegreesMinutesSeconds: qsTr("DMS")
    property string strUsng: qsTr("USNG")
    property string strUtm: qsTr("UTM")

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
            SimpleRenderer {
                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleX
                    color: Qt.rgba(1.0, 0.0, 0.0, 1.0)
                    size: 15.0
                }
            }

            Graphic {
                id: locationGraphic
                Point {
                    x: -117.195723
                    y: 34.056195
                    SpatialReference {
                        wkid: 4326
                    }
                }
                onComponentCompleted: {
                    setTextFromPoint(locationGraphic.geometry);
                }
            }
        }

        onMouseClicked: {  // on MapView
            handleLocationUpdate(mouse.mapPoint);   
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
            onAccepted: {
                handleTextUpdate(strUsng, text);
            }
        }
    }

    function handleLocationUpdate(point) {
        if (! point.isEmpty)
        {
            locationGraphic.geometry = point;
            setTextFromPoint(point);
        }
    }

    function handleTextUpdate(textType, text) {
        var point = createPointFromText(textType, text);
        if (point)
            handleLocationUpdate(point);
    }

    function setTextFromPoint(point) {
        if (point.isEmpty)
            return;
        //! [FormatCoordinates CoordinateFormatter point to text]
        var decimalPlaces = 6;
        textDD.text   = CoordinateFormatter.toLatitudeLongitude(point, Enums.LatitudeLongitudeFormatDecimalDegrees, decimalPlaces);

        decimalPlaces = 1;
        textDMS.text  = CoordinateFormatter.toLatitudeLongitude(point, Enums.LatitudeLongitudeFormatDegreesMinutesSeconds, decimalPlaces);

        var addSpaces = true;
        textUsng.text = CoordinateFormatter.toUsng(point, 5, addSpaces);

        textUtm.text  = CoordinateFormatter.toUtm(point, Enums.UtmConversionModeLatitudeBandIndicators, addSpaces);
        //! [FormatCoordinates CoordinateFormatter point to text]
    }

    function createPointFromText(textType, text) {
        if (strDecimalDegrees === textType
                || strDegreesMinutesSeconds === textType) {
            //! [FormatCoordinates CoordinateFormatter text to point]
            return CoordinateFormatter.fromLatitudeLongitude(text, map.spatialReference);
            //! [FormatCoordinates CoordinateFormatter text to point]
        }
        if (strUsng === textType) {
            return CoordinateFormatter.fromUsng(text, map.spatialReference);
        }
        if (strUtm === textType) {
            return CoordinateFormatter.fromUtm(text, map.spatialReference, Enums.UtmConversionModeLatitudeBandIndicators);
        }
    }
}
