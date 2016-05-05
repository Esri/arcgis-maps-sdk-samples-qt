// [WriteFile Name=VectorTiledLayerUrl, Category=Layers]
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Create MapView that contains a Map
    MapView {
        id: mapview
        anchors.fill: parent
        Map {
            id: map
            Basemap {
                // Nest an ArcGISVectorTiledLayer Layer in the Basemap
                ArcGISVectorTiledLayer {
                    url: "http://www.arcgis.com/home/item.html?id=00cd8e843bae49b3a040423e5d65416b"
                }
            }
            initialViewpoint: ViewpointCenter {
                center: Point { x:-80.18; y: 25.778135; spatialReference: SpatialReference { wkid: 4326 } }
                scale: 150000
            }
        }
    }

    ComboBox {
        id: comboBoxBasemap
        anchors {
            left: parent.left
            top: parent.top
            margins: 15 * scaleFactor
        }
        width: 175 * scaleFactor
        model: ["Navigation","Streets","Night","Dark Gray"]
        onCurrentTextChanged: {
            // Call this JavaScript function when the current selection changes
            if (map.loadStatus === Enums.LoadStatusLoaded)
                changeBasemap();
        }

        function changeBasemap() {
            // Determine the selected basemap, create that type, and set the Map's basemap
            var layer;
            switch (comboBoxBasemap.currentText) {
            case "Navigation":
            default:
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"http://www.arcgis.com/home/item.html?id=00cd8e843bae49b3a040423e5d65416b"});
                break;
            case "Streets":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"http://www.arcgis.com/home/item.html?id=3b8814f6ddbd485cae67e8018992246e"});
                break;
            case "Night":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"http://www.arcgis.com/home/item.html?id=f96366254a564adda1dc468b447ed956"});
                break;
            case "Dark Gray":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"http://www.arcgis.com/home/item.html?id=3e3099d7302f4d99bc6f916dcc07ed59"});
                break;
            }
            var newBasemap = ArcGISRuntimeEnvironment.createObject("Basemap");
            newBasemap.baseLayers.append(layer);
            map.basemap = newBasemap;
        }
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
