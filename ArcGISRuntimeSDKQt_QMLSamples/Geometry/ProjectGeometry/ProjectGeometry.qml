// [WriteFile Name=ProjectGeometry, Category=Geometry]
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
import Esri.ArcGISRuntime.Toolkit.Controls 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapNationalGeographic {}

            // Set initial viewpoint to Minneapolis
            ViewpointExtent {
                Envelope {
                    xMin: -10995912.335747
                    yMin: 5267868.874421
                    xMax: -9880363.974046
                    yMax: 5960699.183877
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        // Declare a callout
        Callout {
            id: callout
            calloutData: parent.calloutData
            accessoryButtonHidden: true
            autoAdjustWidth: true
            maxWidth: 350
            leaderPosition: leaderPositionEnum.Automatic
        }

        // create a graphics to show the input location
        GraphicsOverlay {

            // create a red marker symbol for the input point
            Graphic {
                id: inputGraphic

                SimpleMarkerSymbol {
                    color: "red"
                    size: 5
                    style: Enums.SimpleMarkerSymbolStyleCircle
                }
            }
        }

        // handle mouseClicked signal
        onMouseClicked: {
            // get the mouse click as a point
            const originalPoint = mouse.mapPoint;

            // show the clicked location on the map with a graphic
            inputGraphic.geometry = originalPoint;

            // create the output SpatialReference by specifying a well known ID (WKID)
            const spatialReference = ArcGISRuntimeEnvironment.createObject("SpatialReference", {
                                                                               wkid: 4326
                                                                           });

            // project the web mercator point to WGS84
            const projectedPoint = GeometryEngine.project(originalPoint, spatialReference);

            // update callout data
            calloutData.title = "Coordinates"
            calloutData.location = originalPoint;
            const ox = originalPoint.x.toFixed(5)
            const oy = originalPoint.y.toFixed(5);
            const px = projectedPoint.x.toFixed(5);
            const py = projectedPoint.y.toFixed(5);
            calloutData.detail = "Original: %1, %2\nProjected: %3, %4".arg(ox).arg(oy).arg(px).arg(py);

            // show the callout
            callout.showCallout();
        }
    }
}
