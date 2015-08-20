// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Create MapView that contains a Map with the Imagery with Labels Basemap
    MapView {
        anchors.fill: parent
        Map {
            BasemapImageryWithLabels {}
            // Set the initialViewpoint property to a ViewpointCenter object
            initialViewpoint: viewpoint
        }
    }

    // Create the intial Viewpoint
    ViewpointCenter {
        id: viewpoint
        // Specify the center Point
        center: Point {
            x: -7122777.61840761
            y: -4011076.1090391986
            spatialReference: SpatialReference { wkid: 102100 }
        }
        // Specify the scale
        scale: 15000
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
