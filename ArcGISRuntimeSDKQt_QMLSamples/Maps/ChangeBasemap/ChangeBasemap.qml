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
import Esri.ArcGISRuntime 100.10

Rectangle {
    width: 800
    height: 600

    // Create MapView that contains a Map
    MapView {
        anchors.fill: parent
        Map {
            id: map
            // Set the initial basemap to Topographic
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
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
            ListElement { text: "Topographic"; map: "BasemapStyleArcGISTopographic" }
            ListElement { text: "Streets"; map: "BasemapStyleArcGISStreets" }
            ListElement { text: "Streets - Relief"; map: "BasemapStyleArcGISStreetsRelief" }
            ListElement { text: "Streets - Night"; map: "BasemapStyleArcGISStreetsNight" }
            ListElement { text: "Imagery"; map: "BasemapStyleArcGISImageryStandard" }
            ListElement { text: "Imagery with Labels"; map: "BasemapStyleArcGISImagery" }
            ListElement { text: "Labels without Imagery"; map: "BasemapStyleArcGISImageryLabels" }
            ListElement { text: "Dark Gray Canvas"; map: "BasemapStyleArcGISDarkGray" }
            ListElement { text: "Light Gray Canvas"; map: "BasemapStyleArcGISLightGray" }
            ListElement { text: "Navigation"; map: "BasemapStyleArcGISNavigation" }
            ListElement { text: "OpenStreetMap"; map: "BasemapStyleOsmStandard" }
            ListElement { text: "Oceans"; map: "BasemapStyleArcGISOceans" }
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

        function changeBasemap(style) {
            map.basemap = ArcGISRuntimeEnvironment.createObject("Basemap", {
                                                                initStyle: style
                                                                });
        }
    }
}
