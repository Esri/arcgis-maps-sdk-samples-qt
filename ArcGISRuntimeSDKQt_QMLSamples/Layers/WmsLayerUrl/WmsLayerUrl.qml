// [WriteFile Name=WmsLayerUrl, Category=Layers]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapLightGrayCanvas {}

            // Add a WMS Layer by specifying the URL and layer name
            WmsLayer {
                url: "https://nowcoast.noaa.gov/arcgis/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/WMSServer?request=GetCapabilities&service=WMS"
                layerNames: ["1"]
            }

            // start zoomed in over the US
            ViewpointExtent {
                Envelope {
                    xMin: -19195297.778679
                    yMin: 512343.939994
                    xMax: -3620418.579987
                    yMax: 8658913.035426
                    SpatialReference {
                        wkid: 3857
                    }
                }
            }
        }
    }
}
