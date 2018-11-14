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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600
    
    //! [display vector tiled layer]
    // Create MapView that contains a Map
    MapView {
        id: mapview
        anchors.fill: parent
        Map {
            id: map
            Basemap {
                // Nest an ArcGISVectorTiledLayer Layer in the Basemap
                ArcGISVectorTiledLayer {
                    url: "https://www.arcgis.com/home/item.html?id=7675d44bb1e4428aa2c30a9b68f97822"
                }
            }
            initialViewpoint: ViewpointCenter {
                center: Point { x:-80.18; y: 25.778135; spatialReference: SpatialReference { wkid: 4326 } }
                targetScale: 150000
            }
        }
    }
    //! [display vector tiled layer]

    ComboBox {
        id: comboBoxBasemap
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width
        model: ["Mid-Century","Colored Pencil","Newspaper","Nova", "World Street Map (Night)"]
        onCurrentTextChanged: {
            // Call this JavaScript function when the current selection changes
            if (map.loadStatus === Enums.LoadStatusLoaded)
                changeBasemap();
        }

        Component.onCompleted : {
            for (var i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width);
            }
        }

        TextMetrics {
            id: metrics
            font: comboBoxBasemap.font
        }

        function changeBasemap() {
            // Determine the selected basemap, create that type, and set the Map's basemap
            var layer;
            switch (comboBoxBasemap.currentText) {
            case "Mid-Century":
            default:
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"https://www.arcgis.com/home/item.html?id=7675d44bb1e4428aa2c30a9b68f97822"});
                break;
            case "Colored Pencil":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"https://www.arcgis.com/home/item.html?id=4cf7e1fb9f254dcda9c8fbadb15cf0f8"});
                break;
            case "Newspaper":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"https://www.arcgis.com/home/item.html?id=dfb04de5f3144a80bc3f9f336228d24a"});
                break;
            case "Nova":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"https://www.arcgis.com/home/item.html?id=75f4dfdff19e445395653121a95a85db"});
                break;
            case "World Street Map (Night)":
                layer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {url:"https://www.arcgis.com/home/item.html?id=86f556a2d1fd468181855a35e344567f"});
                break;
            }
            var newBasemap = ArcGISRuntimeEnvironment.createObject("Basemap");
            newBasemap.baseLayers.append(layer);
            map.basemap = newBasemap;
        }
    }
}
