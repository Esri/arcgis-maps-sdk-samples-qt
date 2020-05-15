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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600

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
            margins: 5
        }
        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width
        textRole: "text"
        model: ListModel {
            ListElement { text: "Topographic"; map: "BasemapTopographic" }
            ListElement { text: "Streets"; map: "BasemapStreets" }
            ListElement { text: "Streets (Vector)"; map: "BasemapStreetsVector" }
            ListElement { text: "Streets - Night (Vector)"; map: "BasemapStreetsNightVector" }
            ListElement { text: "Imagery (Raster)"; map: "BasemapImagery" }
            ListElement { text: "Imagery with Labels (Raster)"; map: "BasemapImageryWithLabels" }
            ListElement { text: "Imagery with Labels (Vector)"; map: "BasemapImageryWithLabelsVector" }
            ListElement { text: "Dark Gray Canvas (Vector)"; map: "BasemapDarkGrayCanvasVector" }
            ListElement { text: "Light Gray Canvas (Raster)"; map: "BasemapLightGrayCanvas" }
            ListElement { text: "Light Gray Canvas (Vector)"; map: "BasemapLightGrayCanvasVector" }
            ListElement { text: "Navigation (Vector)"; map: "BasemapNavigationVector" }
            ListElement { text: "OpenStreetMap (Raster)"; map: "BasemapOpenStreetMap" }
            ListElement { text: "Oceans"; map: "BasemapOceans" }
        }

        Component.onCompleted : {
            for (let i = 0; i < model.count; ++i) {
                metrics.text = model.get(i).text;
                modelWidth = Math.max(modelWidth, metrics.width);
            }
        }

        TextMetrics {
            id: metrics
            font: comboBoxBasemap.font
        }

        onCurrentIndexChanged: {
            // Call this JavaScript function when the current selection changes
            if (map.loadStatus === Enums.LoadStatusLoaded)
                changeBasemap(model.get(currentIndex).map);
        }

        function changeBasemap(type) {
            map.basemap = ArcGISRuntimeEnvironment.createObject(type);
        }
    }
}
