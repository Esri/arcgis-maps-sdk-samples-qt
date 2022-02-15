// [WriteFile Name=BrowseBuildingFloors, Category=Maps]
// [Legal]
// Copyright 2022 Esri.

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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property FloorManager floorManager;

    MapView {
        id: mapView
        anchors.fill: parent

        PortalItem {
            id: portalItem
            itemId: "f133a698536f44c8884ad81f80b6cfc7"
            Component.onCompleted: load();
            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded) {
                    return;
                }
            }
        }

        Map {
            id: arcgisOnlineMap
            item: portalItem

            onLoadStatusChanged: {
                if (arcgisOnlineMap.loadStatus === Enums.LoadStatusLoaded) {
                    arcgisOnlineMap.floorManager.load();
                }
            }
        }

        Column {
            spacing: 15
            padding: 10

            ComboBox {
                id: floor_level
                width: 200
                model: ["Level 1", "Level 2", "Level 3"]
            }

            Button {
                id: select_floor
                width: 200
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Select Floor"
                onClicked: {
                    if (arcgisOnlineMap.loadStatus === Enums.LoadStatusLoaded) {
                        arcgisOnlineMap.floorManager.load();
                    }

                    const levels = arcgisOnlineMap.floorManager.levels;

                    if (floor_level.currentText == "Level 1")
                    {
                        // Set the floor to level 1
                        levels[0].visible = true

                        // Make the other floors invisible
                        levels[1].visible = false
                        levels[2].visible = false
                    }

                    if (floor_level.currentText == "Level 2")
                    {
                        // Set the floor to level 2
                        levels[1].visible = true

                        // Make the other floors invisible
                        levels[0].visible = false
                        levels[2].visible = false
                    }

                    if (floor_level.currentText == "Level 3")
                    {
                        // Set the floor to level 3
                        levels[2].visible = true

                        // Make the other floors invisible
                        levels[0].visible = false
                        levels[1].visible = false
                    }
                }
            }
        }

    }
}

