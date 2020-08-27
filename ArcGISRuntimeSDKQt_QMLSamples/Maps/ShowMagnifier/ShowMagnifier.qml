// [WriteFile Name=ShowMagnifier, Category=Maps]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600

    // create MapView that contains a Map with the Imagery basemap
    MapView {
        id: mapView
        anchors.fill: parent
        // set the magnifier to true
        magnifierEnabled: true

        Map {
            BasemapImagery {}

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(initialViewpoint);
                }
            }
        }
    }

    // create the initial viewpoint
    ViewpointCenter {
        id: initialViewpoint
        center: Point {
            x: -110.8258
            y: 32.1545089
            spatialReference: SpatialReference {wkid: 4326}
        }
        targetScale: 2e4
    }
}
