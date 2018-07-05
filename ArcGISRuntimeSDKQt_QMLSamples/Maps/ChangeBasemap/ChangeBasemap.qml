// [WriteFile Name=ChangeBasemap, Category=Maps]
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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Create MapView that contains a Map
    MapView {
        anchors.fill: parent
        Map {
            id: map
            // Set the initial basemap to Topographic
            BasemapTopographic {}
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
        model: ["Topographic","Streets",
            "Streets (Vector)",
            "Streets - Night (Vector)",
            "Imagery (Raster)",
            "Imagery with Labels (Raster)",
            "Imagery with Labels (Vector)",
            "Dark Gray Canvas (Vector)",
            "Light Gray Canvas (Raster)",
            "Light Gray Canvas (Vector)",
            "Navigation (Vector)",
            "OpenStreetMap (Raster)",
            "Oceans"]
        onCurrentTextChanged: {
            // Call this JavaScript function when the current selection changes
            if (map.loadStatus === Enums.LoadStatusLoaded)
                changeBasemap();
        }

        function changeBasemap() {
            // Determine the selected basemap, create that type, and set the Map's basemap
            switch (comboBoxBasemap.currentText) {
            case "Topographic":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapTopographic");
                break;
            case "Streets (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreetsVector");
                break;

            case "Streets - Night (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreetsNightVector");
                break;
            case "Imagery (Raster)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapImagery");
                break;
            case "Imagery with Labels (Raster)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapImageryWithLabels");
                break;
            case "Imagery with Labels (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapImageryWithLabelsVector");
                break;
            case "Dark Gray Canvas (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapDarkGrayCanvasVector");
                break;
            case "Light Gray Canvas (Raster)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapLightGrayCanvas");
                break;
            case "Light Gray Canvas (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapLightGrayCanvasVector");
                break;

            case "Navigation (Vector)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapNavigationVector");
                break;
            case "OpenStreetMap (Raster)":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapOpenStreetMap");
                break;
            case "Oceans":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapOceans");
                break;
            default:
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapTopographic");
                break;
            }
        }
    }
}
