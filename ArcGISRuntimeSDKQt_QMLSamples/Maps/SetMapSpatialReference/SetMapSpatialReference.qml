// [WriteFile Name=SetMapSpatialReference, Category=Maps]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

    MapView {
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            id: map
            // Specify the SpatialReference
            spatialReference: SpatialReference { wkid:54024 }
        }
    }

    Basemap {
        id: basemap
        ArcGISMapImageLayer {
            url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer"
        }
    }

    Component.onCompleted: {
        map.basemap = basemap;
    }
}
