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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.13
import Esri.ArcGISRuntime 100.15

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property FeatureTable positioningTable: null
    property ArcGISFeatureTable pathwaysTable: null
    property Field dateCreatedField: null
    property string globalId: ""
    property int currentFloor: -1

    onPositioningTableChanged: setupIndoors()
    onPathwaysTableChanged: setupIndoors()
    onGlobalIdChanged: setupIndoors()
    onCurrentFloorChanged: changeFloorDisplay()

    function setupIndoors() {
        if (!positioningTable || !pathwaysTable || !globalId)
            return;

        const indoorsLDS = ArcGISRuntimeEnvironment.createObject("IndoorsLocationDataSource",
                                                                 {
                                                                     positioningTable: positioningTable,
                                                                     pathwaysTable: pathwaysTable,
                                                                     positioningId: globalId
                                                                 });

        indoorsLDS.statusChanged.connect(()=>
                                         {
                                             console.log("IndoorsLocationDataSource statusChanged", indoorsLDS.started);
                                         });

        indoorsLDS.errorChanged.connect(()=>
                                        {
                                            console.log("IndoorsLocationDataSource errorChanged", indoorsLDS.error.message, indoorsLDS.error.additionalMessage);
                                        });

        mapView.locationDisplay.locationChanged.connect(() => {
                                                            currentFloor = mapView.locationDisplay.location.additionalSourceProperties.floor;

                                                            floorText.text = "Floor: " + currentFloor;
                                                            positionSourceText.text = "Position Source: " + mapView.locationDisplay.location.additionalSourceProperties.positionSource;
                                                            transmitterCountText.text = "Transmitter Count: " + mapView.locationDisplay.location.additionalSourceProperties.satelliteCount;
                                                            horizontalAccuracyText.text = "Horizontal Accuracy: " + mapView.locationDisplay.location.horizontalAccuracy.toFixed(2) + " m";
                                                        });

        mapView.locationDisplay.dataSource = indoorsLDS;
        mapView.locationDisplay.start();
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            PortalItem {
                itemId: "89f88764c29b48218366855d7717d266"
                portal: Portal {
                    url: "https://viennardc.maps.arcgis.com"
                    credential: Credential {
                        username: "tester_viennardc"
                        password: "password.testing12345"
                    }
                }
            }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                console.log("loaded");

                findPositioningTable("ips_positioning")
                findPathwaysTable("Pathways");
            }
        }
    }

    Rectangle {
        id: debugInfoRectangle
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
                id: floorText
                text: "Floor: undefined"
            }
            Text {
                id: positionSourceText
                text: "Position source: undefined"
            }
            Text {
                id: transmitterCountText
                text: "Transmitter count: 0"
            }
            Text {
                id: horizontalAccuracyText
                text: "Horizontal accuracy: undefined"
            }
        }
    }

    function findPositioningTable(tableName) {
        map.tables.forEach((table) => {
                               table.loadStatusChanged.connect(() => {
                                                                   if (table.loadStatus === Enums.LoadStatusLoaded && table.tableName === tableName) {
                                                                       console.log("found the ips_positioning table!")
                                                                       positioningTable = table;
                                                                       findGlobalId(["DateCreated", "DATE_CREATED"]);
                                                                   }
                                                               });
                               table.load();
                           });
    }

    function findPathwaysTable(layerName) {
        for (let i = 0; i < map.operationalLayers.count; ++i) {
            const layer = map.operationalLayers.get(i);
            if (layer.name === layerName && layer.layerType === Enums.LayerTypeFeatureLayer) {
                const table = layer.featureTable;
                if (table.featureTableType === Enums.FeatureTableTypeServiceFeatureTable) {
                    console.log("Found the pathways table");
                    pathwaysTable = table;
                    return;
                }
            }
        }
    }

    QueryParameters {
        id: globalIdQueryParameters
        maxFeatures: 1
        whereClause: "1=1"
    }

    function findGlobalId(globalIdSortNames) {
        const fields = positioningTable.fields;
        for (let i = 0; i < fields.length; i++) {
            globalIdSortNames.forEach((sortName) => {
                                          if (sortName.toLowerCase() === fields[i].name.toLowerCase()) {
                                              dateCreatedField = fields[i];
                                              const orderByField = ArcGISRuntimeEnvironment.createObject("OrderBy", {fieldName: fields[i].name, sortOrder: Enums.SortOrderDescending});
                                              globalIdQueryParameters.orderByFields = [orderByField];
                                          }
                                      });
        }

        positioningTable.queryFeaturesResultChanged.connect(() => {
                                                                globalId = positioningTable.queryFeaturesResult.iterator.features[0].attributes.attributesJson.GlobalID;
                                                                console.log("Found the global id!");
                                                            });

        positioningTable.queryFeatures(globalIdQueryParameters);
    }

    function changeFloorDisplay() {
        const layers = map.operationalLayers;
        layers.forEach((layer) => {
                           if (["Details", "Units", "Levels"].indexOf(layer.name) >= 0) {
                                   layer.definitionExpression = "VERTICAL_ORDER = " + currentFloor;
                           }
                       });
    }
}
