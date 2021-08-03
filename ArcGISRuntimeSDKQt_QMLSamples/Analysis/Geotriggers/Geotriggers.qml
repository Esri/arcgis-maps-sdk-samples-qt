// [WriteFile Name=Geotriggers, Category=Analysis]
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
import Esri.ArcGISRuntime 100.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            PortalItem {
                itemId: "6ab0e91dc39e478cae4f408e1a36a308"
            }
            ServiceFeatureTable {
                id: gardenSections
                PortalItem {
                    itemId: "1ba816341ea04243832136379b8951d9"
                }
                initLayerId: "0"
            }
            ServiceFeatureTable {
                id: gardenPois
                PortalItem {
                    itemId: "7c6280c290c34ae8aeb6b5c4ec841167"
                }
                initLayerId: "0"
            }
        }
        locationDisplay {
            id: userLocation
            autoPanMode: Enums.LocationDisplayAutoPanModeRecenter
            initialZoomScale: 1000
            started: true
            dataSource: SimulatedLocationDataSource {

            }
        }
    }
}
