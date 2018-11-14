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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

    // Declare a map view inside the rectangle
    MapView {
        id: mapView

        anchors.fill: parent

        // Nest a map inside of the map view
        Map {
            id: map
            // set the basemap
            BasemapTopographic {}
        }

        // Add a graphics overlay to the map view
        GraphicsOverlay {
            id: graphicsOverlay
            // assign a render to the graphics overlay
            renderer: SimpleRenderer {
                symbol: SimpleFillSymbol {
                    style: Enums.SimpleFillSymbolStyleSolid
                    color: Qt.rgba(1, 1, 0, 0.7)
                }
            }
        }

        //! [identify graphics api snippet]
        // Signal handler for mouse click event on the map view
        onMouseClicked: {
            var tolerance = 22;
            var returnPopupsOnly = false;
            var maximumResults = 1000;
            mapView.identifyGraphicsOverlayWithMaxResults(graphicsOverlay, mouse.x, mouse.y, tolerance, returnPopupsOnly, maximumResults);
        }

        // Signal handler for identify graphics overlay
        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus === Enums.TaskStatusCompleted) {
                if (identifyGraphicsOverlayResult.graphics.length > 0) {
                    msgDialog.open();
                }
            } else if (identifyGraphicsOverlayStatus === Enums.TaskStatusErrored) {
                console.log("error");
            }
        }
        //! [identify graphics api snippet]
    }

    Dialog {
        id: msgDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        property alias text : textLabel.text
        Text {
            id: textLabel
            text: "Tapped on graphic"
        }
    }

    Component.onCompleted: {
        // create the polygon by assigning points
        var polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder", {spatialReference: SpatialReference.createWebMercator()});
        polygonBuilder.addPointXY(-20e5, 20e5);
        polygonBuilder.addPointXY(20e5, 20e5);
        polygonBuilder.addPointXY(20e5, -20e5);
        polygonBuilder.addPointXY(-20e5, -20e5);
        // assign the geometry of the graphic to be the polygon
        var polygonGraphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        polygonGraphic.geometry = polygonBuilder.geometry;
        // add the graphic to the polygon graphics overlay
        graphicsOverlay.graphics.append(polygonGraphic);
    }
}
