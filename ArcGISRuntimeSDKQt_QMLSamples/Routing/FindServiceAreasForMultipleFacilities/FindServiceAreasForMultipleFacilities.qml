// [WriteFile Name=FindServiceAreasForMultipleFacilities, Category=Routing]
// [Legal]
// Copyright 2020 Esri.

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
import Esri.ArcGISRuntime 100.9
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.2

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property url featureTableUrl: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0"
    property url imageUrl: "https://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png"
    property url serviceTaskAreaUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ServiceArea"
    property list<SimpleFillSymbol> fillSymbols: [
        SimpleFillSymbol {
            style: Enums.SimpleFillSymbolStyleSolid
            color: "#66FFA500"
            outline: null
        },
        SimpleFillSymbol {
            style: Enums.SimpleFillSymbolStyleSolid
            color: "#66FF0000"
        }
    ]

    MapView {
        id: mapView
        anchors.fill: parent

        GraphicsOverlay {
            id: serviceAreasOverlay
        }

        Map {
            BasemapLightGrayCanvas {}

            // create query parameters used to select all facilities from the feature table
            QueryParameters {
                id: queryParameters
                whereClause: "1=1"
            }

            FeatureLayer {
                featureTable: ServiceFeatureTable {
                    id: featureTable
                    url: featureTableUrl
                }
                onLoadStatusChanged: {
                    if(loadStatus === Enums.LoadStatusLoaded) {
                        mapView.setViewpointGeometryAndPadding(fullExtent, 100);
                    }
                }
                onErrorChanged: {
                    console.warn(error.message);
                }

                renderer: SimpleRenderer {
                    PictureMarkerSymbol {
                        url: imageUrl
                        height: 25
                        width: 25
                    }
                }
            }

            ServiceAreaTask {
                id: serviceAreaTask
                url: serviceTaskAreaUrl

                Component.onCompleted: {
                    load();
                }

                onErrorChanged: {
                    console.warn(error.message);
                }

                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded) {
                        createDefaultParameters();
                    }
                }

                onCreateDefaultParametersStatusChanged: {
                    if (createDefaultParametersStatus === Enums.TaskStatusCompleted) {
                        createDefaultParametersResult.polygonDetail = Enums.ServiceAreaPolygonDetailHigh;
                        createDefaultParametersResult.returnPolygons = true;

                        // add service areas of 1 minute and 3 minutes travel time by car
                        createDefaultParametersResult.defaultImpedanceCutoffs = [1.0, 3.0];
                        createDefaultParametersResult.setFacilitiesWithFeatureTable(featureTable, queryParameters);
                    }
                }

                onSolveServiceAreaStatusChanged: {
                    if (solveServiceAreaStatus === Enums.TaskStatusCompleted) {
                        // iterate through the facilities to get the service area polygons
                        for (let i = 0; i < solveServiceAreaResult.facilities.length; i++) {
                            const serviceAreaPolygonList = solveServiceAreaResult.resultPolygons(i);
                            //create a graphic for each available polygon
                            for (let j = 0; j < serviceAreaPolygonList.length; j++) {
                                serviceAreasOverlay.graphics.append(ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: serviceAreaPolygonList[j].geometry, symbol: fillSymbols[j]}));
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            anchors {
                left: parent.left
                top: parent.top
            }

            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: "Find service areas"
                onClicked: {
                    enabled = false;
                    serviceAreaTask.solveServiceArea(serviceAreaTask.createDefaultParametersResult)
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: true
        running: serviceAreaTask.solveServiceAreaStatus === Enums.TaskStatusInProgress
    }
}
