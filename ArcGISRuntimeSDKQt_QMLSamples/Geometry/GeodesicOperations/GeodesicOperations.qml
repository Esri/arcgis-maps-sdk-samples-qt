// [WriteFile Name=GeodesicOperations, Category=Geometry]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property string distanceText

    MapView {
        id: mapView
        anchors.fill: parent

        // create a graphics overlay
        GraphicsOverlay {

            // add a marker graphic for NYC
            Graphic {
                id: nycGraphic
                Point {
                    x: -73.7781
                    y: 40.6413
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                symbol: markerSymbol
            }

            // add a graphic for the path
            Graphic {
                id: pathGraphic
                symbol: pathSymbol
            }

            // add an end marker graphic
            Graphic {
                id: destinationGraphic
                symbol: markerSymbol
            }
        }

        // create a map with the imagery basemap
        Map {
            BasemapImagery {}
        }

        // handle the mouse click
        onMouseClicked: {
            // re-project the point to match the NYC graphic
            const destination = GeometryEngine.project(mouse.mapPoint, nycGraphic.geometry.spatialReference);

            // update the destination graphic
            destinationGraphic.geometry = destination;

            // create line with start/end points
            const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {spatialReference: destination.spatialReference});
            polylineBuilder.addPoints([nycGraphic.geometry, destinationGraphic.geometry]);
            const polyline = polylineBuilder.geometry;

            // densify the path as a geodesic curve and show it with the path graphic
            const maxSegmentLength = 1;
            const unitOfMeasurement = ArcGISRuntimeEnvironment.createObject("LinearUnit", {linearUnitId: Enums.LinearUnitIdKilometers});
            const curveType = Enums.GeodeticCurveTypeGeodesic;
            const pathGeometry = GeometryEngine.densifyGeodetic(polyline, maxSegmentLength, unitOfMeasurement, curveType);

            // update the graphic
            pathGraphic.geometry = pathGeometry;

            // calculate the path's geodetic length
            distanceText = GeometryEngine.lengthGeodetic(pathGeometry, unitOfMeasurement, curveType).toFixed(2);
        }
    }    

    // create a line symbol
    SimpleLineSymbol {
        id: pathSymbol
        color: "blue"
        width: 5
        style: Enums.SimpleLineSymbolStyleDash
    }

    // create a marker symbol
    SimpleMarkerSymbol {
        id: markerSymbol
        color: "blue"
        style: Enums.SimpleMarkerSymbolStyleCircle
        size: 10
    }

    // display text for geodesic length
    Text {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }

        text: "%1 kilometers".arg(distanceText)
        visible: distanceText.length > 0
        font.pixelSize: 20
        color: "white"
    }
}
