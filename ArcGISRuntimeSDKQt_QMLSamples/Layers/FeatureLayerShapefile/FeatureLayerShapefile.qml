// [WriteFile Name=FeatureLayerShapefile, Category=Layers]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/shp/"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapStreetsVector {}

            FeatureLayer {

                ShapefileFeatureTable {
                    path: dataPath + "Public_Art.shp"
                }

                onLoadStatusChanged: {
                     if (loadStatus !== Enums.LoadStatusLoaded)
                         return;

                     mapView.setViewpointCenterAndScale(fullExtent.center, 80000);
                 }

            }
        }
    }
}
