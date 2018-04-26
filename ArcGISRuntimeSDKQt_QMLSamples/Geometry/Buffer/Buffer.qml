// [WriteFile Name=Buffer, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // declare a map view
    MapView {
        id: mapView
        anchors.fill: parent

        // Add a map with a basemap
        Map {
            BasemapTopographic {}

            // Set an initial viewpoint
            ViewpointExtent {
                Envelope {
                    xMin: -10863035.97
                    yMin: 3838021.34
                    xMax: -10744801.344
                    yMax: 3887145.299
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        // handle the mouse click - perform a buffer on click
        onMouseClicked: bufferPoint(mouse.mapPoint);

        // add a graphics overlay to the map view
        GraphicsOverlay {
            id: graphicsOverlay
        }
    }

    // function to buffer a point and add graphics
    function bufferPoint(point) {
        // Create a variable to be the buffer size in meters. There are 1609.34 meters in one mile.
        var bufferInMeters = bufferSizeText.text * 1609.34;

        // Buffer the point
        var buffer = GeometryEngine.buffer(point, bufferInMeters);

        // Add the result buffer as a graphic
        var resultGraphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        resultGraphic.symbol = bufferSymbol;
        resultGraphic.geometry = buffer;
        graphicsOverlay.graphics.append(resultGraphic);

        // Add the clicked point as a graphic
        var clickedPointGraphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        clickedPointGraphic.symbol = pointSymbol;
        clickedPointGraphic.geometry = point;
        graphicsOverlay.graphics.append(clickedPointGraphic);
    }

    // display a control pane to change size
    Rectangle {
        anchors {
            margins: -5 * scaleFactor
            fill: controlColumn
        }
        color: "lightgray"
        radius: 5 * scaleFactor
        border {
            color: "darkgray"
            width: 1 * scaleFactor
        }
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }
        spacing: 5 * scaleFactor

        Text {
            text: "Buffer size (miles):"
        }

        TextField {
            id: bufferSizeText
            validator: RegExpValidator{ regExp: /^[1-9][0-9]?$|^100$/ }
            text: "5"
        }
    }

    // create a simple fill symbol for the buffer result
    SimpleFillSymbol {
        id: bufferSymbol
        color: Qt.rgba(0, 1, 0, 0.5)
        style: Enums.SimpleFillSymbolStyleSolid

        SimpleLineSymbol {
            color: "green"
            style: Enums.SimpleLineSymbolStyleSolid
            width: 5
        }
    }

    // create a marker symbol for the clicked point
    SimpleMarkerSymbol {
        id: pointSymbol
        color: "red"
        size: 5
        style: Enums.SimpleMarkerSymbolStyleCircle
    }
}
