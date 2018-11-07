// [WriteFile Name=SetInitialMapLocation, Category=Maps]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

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
        targetScale: 15000
    }
}
