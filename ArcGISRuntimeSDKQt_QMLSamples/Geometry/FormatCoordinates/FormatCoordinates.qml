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
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600
    
    property int labelWidth: 100
    property int coordinateTextWidth: 200
    property int fontPixelSize: 14
    property int textPadding: Qt.platform.os === "android" ? (10) : (4)
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
            bottom: formatRect.top
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
        id: formatRect
        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }
        height: childrenRect.height

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

    function handleLocationUpdate(point) {
        if (!point.isEmpty)
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

        // check for invalid points and update the text boxes
        if (textDD.text.length === 0)
            textDD.text = "invalid point";

        if (textDMS.text.length === 0)
            textDMS.text = "invalid point";

        if (textUsng.text.length === 0)
            textUsng.text = "invalid point";

        if (textUtm.text.length === 0)
            textUtm.text = "invalid point";
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
