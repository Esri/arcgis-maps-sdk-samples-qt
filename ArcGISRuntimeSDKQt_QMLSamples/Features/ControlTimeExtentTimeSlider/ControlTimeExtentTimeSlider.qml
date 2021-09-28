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

import QtQuick 2.12
import Esri.ArcGISRuntime 100.13
//import Esri.ArcGISRuntime.Toolkit 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property alias layerListModel: map.operationalLayers

    MapView {
        id: mapView
        anchors.fill: parent


//        TimeSlider {
//                    anchors {
//                        left: parent.left
//                        right: parent.right
//                        bottom: parent.bottom
//                    }

//                    geoView: view
//                }

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            onLoadStatusChanged: {
                            if (loadStatus !== Enums.LoadStatusLoaded)
                                return;

                            // add layers to the map
                            operationalLayers.append(hurricanesLayer);

                            drawOrderModel.setLayerListModel(operationalLayers);
                        }
        }
    }
    ArcGISMapImageLayer {
        id: hurricanesLayer
        url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/Hurricanes/MapServer"
    }
}
