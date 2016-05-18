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
import Esri.ArcGISRuntime 100.0
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
        model: ["Topographic","Streets","Imagery","Oceans"]
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
            case "Streets":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreets");
                break;
            case "Imagery":
                map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapImagery");
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

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
