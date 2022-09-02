// [WriteFile Name=ControlTimeExtentTimeSlider, Category=Features]
// [Legal]
// Copyright 2021 Esri.

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

import QtQuick
import Esri.ArcGISRuntime
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            FeatureLayer {
                ServiceFeatureTable {
                    id: featureLayer
                    url: "https://services5.arcgis.com/N82JbI5EYtAkuUKU/ArcGIS/rest/services/Hurricane_time_enabled_layer_2005_1_day/FeatureServer/0"
                }
            }
        }

        // Add a TimeSlider from the toolkit to the MapView
        TimeSlider {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            geoView: mapView
        }
    }
}
