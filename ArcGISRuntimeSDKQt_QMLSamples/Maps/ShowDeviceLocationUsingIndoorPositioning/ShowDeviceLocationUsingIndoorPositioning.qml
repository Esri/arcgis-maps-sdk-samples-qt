// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property FeatureTable positioningTable: null
    property ArcGISFeatureTable pathwaysTable: null
    property int currentFloor: -1
    property var locationProperties: ({})

    onPositioningTableChanged: setupIndoors()
    onPathwaysTableChanged: setupIndoors()
    onCurrentFloorChanged: changeFloorDisplay()

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

<<<<<<< HEAD
        Component.onDestruction: {
            mapView.locationDisplay.stop();
        }

=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
        Map {
            id: map
            PortalItem {
                itemId: "8fa941613b4b4b2b8a34ad4cdc3e4bba"
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                findPositioningTable("ips_positioning")
                findPathwaysTable("Pathways");
            }
        }
    }

    Rectangle {
        id: locationInformationRectangle
        width: parent.width
        height: textColumn.height + 20
        color: "lightgray"

        ColumnLayout {
            id: textColumn
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }

            Text {
                id: initializingText
                text: "Initializing location data source\nand retrieving user location..."
                visible: true
            }

            Text {
                id: floorText
                visible: false
            }
            Text {
                id: positionSourceText
                visible: false
            }
            Text {
                id: transmitterCountText
                visible: false
            }
            Text {
                id: horizontalAccuracyText
                visible: false
            }
        }
    }

    // An IPS positioning feature table is stored with an IPS-enabled map. Each row in the table contains an indoor positioning file.
    // The IndoorsLocationDataSource will use the most recently created row unless given an alternative GlobalId in the constructor.
    function findPositioningTable(tableName) {
        map.tables.forEach((table) => {
           if (table.loadStatus === Enums.LoadStatusLoaded) {
               if (table.tableName === tableName) {
                   positioningTable = table;
                   return;
               }
           } else {
               table.loadStatusChanged.connect(() => {
                   if (table.loadStatus === Enums.LoadStatusLoaded && table.tableName === tableName) {
                       positioningTable = table;
                   }
               });
               table.load();
           }
       });
    }

    // The pathways table is an ArcGISFeatureTable with line features that represent paths through the indoor space.
    // Locations provided by the IndoorsLocationDataSource are snapped to the lines in this feature class.
    function findPathwaysTable(layerName) {
        for (let i = 0; i < map.operationalLayers.count; ++i) {
            const layer = map.operationalLayers.get(i);
            if (layer.name === layerName && layer.layerType === Enums.LayerTypeFeatureLayer) {
                const table = layer.featureTable;
                if (table.featureTableType === Enums.FeatureTableTypeServiceFeatureTable) {
                    pathwaysTable = table;
                    return;
                }
            }
        }
    }

    function setupIndoors() {
        if (!positioningTable || !pathwaysTable)
            return;

        const indoorsLDS = ArcGISRuntimeEnvironment.createObject("IndoorsLocationDataSource",
                                                                 {
                                                                     positioningTable: positioningTable,
                                                                     pathwaysTable: pathwaysTable
                                                                 });

        // Update information text box and floor view if necessary
        mapView.locationDisplay.locationChanged.connect(() => {
            if (mapView.locationDisplay.location !== undefined) {
                initializingText.visible = false;
            } else {
                initializingText.visible = true;
            }

            if (mapView.locationDisplay.location.additionalSourceProperties.floor !== undefined) {
                currentFloor = mapView.locationDisplay.location.additionalSourceProperties.floor;
                floorText.text = "Floor: " + currentFloor;
                floorText.visible = true;
            } else {
                floorText.visible = false;
            }

            if (mapView.locationDisplay.location.additionalSourceProperties.positionSource !== undefined) {
                positionSourceText.text = "Position Source: " + mapView.locationDisplay.location.additionalSourceProperties.positionSource;
                positionSourceText.visible = true;
            } else {
                positionSourceText.visible = false;
            }

            if (mapView.locationDisplay.location.additionalSourceProperties.satelliteCount !== undefined) {
                transmitterCountText.text = "Transmitter Count: " + mapView.locationDisplay.location.additionalSourceProperties.satelliteCount;
                transmitterCountText.visible = true;
            } else {
                transmitterCountText.visible = false;
            }

            if (mapView.locationDisplay.location.horizontalAccuracy !== undefined) {
                horizontalAccuracyText.text = "Horizontal accuracy: " + mapView.locationDisplay.location.horizontalAccuracy.toFixed(2) + " m";
                horizontalAccuracyText.visible = true;
            } else {
                horizontalAccuracyText.visible = false;
            }
        });

        mapView.locationDisplay.dataSource = indoorsLDS;
        mapView.locationDisplay.start();
    }

    // Toggle the visibility of floor features depending on the user's current floor
    function changeFloorDisplay() {
        const layers = map.operationalLayers;
        layers.forEach((layer) => {
            if (["Details", "Units", "Levels"].includes(layer.name)) {
                layer.definitionExpression = "VERTICAL_ORDER = " + currentFloor;
            }
        });
    }
}
