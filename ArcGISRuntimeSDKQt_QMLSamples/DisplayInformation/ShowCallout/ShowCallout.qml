// [WriteFile Name=ShowCallout, Category=DisplayInformation]
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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISRuntime.Toolkit.Controls 2.0

Rectangle {
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent

        Map {
            BasemapTopographic {}

            // initial Viewpoint
            ViewpointCenter {
                Point {
                    x: -1.2e7
                    y: 5e6
                    spatialReference: SpatialReference.createWebMercator()
                }
                scale: 1e7
            }

            // initialize calloutData when map is loaded
            onLoadStatusChanged: {
                if(loadStatus === Enums.LoadStatusLoaded)
                {
                    mapView.calloutData.imageUrl = "qrc:/Samples/DisplayInformation/ShowCallout/RedShinyPin.png";
                    mapView.calloutData.title = "Location";
                }
            }
        }

        Callout {
            id: calling
            calloutData: mapView.calloutData
        }

        onMouseClicked: {
            if (calling.calloutVisible)
                calling.dismiss()
            else
            {
                mapView.calloutData.location = mouse.mapPoint;
                mapView.calloutData.detail = "x: " + mouse.mapPoint.x.toFixed(2) + " y: " + mouse.mapPoint.y.toFixed(2);
                calling.showCallout()
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
