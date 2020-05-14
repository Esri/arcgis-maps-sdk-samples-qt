// [WriteFile Name=FindClosestFacilityToMultipleIncidentsService, Category=Routing]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool busy: false
    property var mySetViewpointTaskId
    property var facilityParams: null
    signal bothDoneLoading(var facilityLoadStatus, var incidentLoadStatus)
    property bool layersLoaded : (facilitiesLayer.loadStatus === Enums.LoadStatusLoaded) && (incidentsLayer.loadStatus === Enums.LoadStatusLoaded)

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapStreetsWithReliefVector {}

            FeatureLayer {
                id: facilitiesLayer
                ServiceFeatureTable {
                    id: facilitiesFeatureTable
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0"
                }
                renderer: SimpleRenderer {
                    symbol: facilitySymbol
                }
                onLoadStatusChanged: setViewpointGeometry();
            }

            FeatureLayer {
                id: incidentsLayer
                ServiceFeatureTable {
                    id: incidentsFeatureTable
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Incidents/FeatureServer/0"
                }
                renderer: SimpleRenderer {
                    symbol: incidentSymbol
                }
                onLoadStatusChanged: setViewpointGeometry();
            }
            Component.onCompleted: busy = true;
        }

        GraphicsOverlay {
            id: resultsOverlay
        }

        Rectangle {
            anchors {
                margins: 5
                left: parent.left
                top: parent.top
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#000000"
            opacity: .70
            radius: 5

            // catch mouse signals from propagating to parent
            MouseArea {
                anchors.fill: parent
                onClicked: mouse.accepted = true
                onWheel: wheel.accepted = true
            }

            ColumnLayout {
                Button {
                    id: solveButton
                    text: qsTr("Solve Routes")
                    Layout.margins: 2
                    Layout.fillWidth: true
                    enabled: false
                    onClicked: {
                        busy = true;
                        solveButton.enabled = false;
                        task.solveClosestFacility(facilityParams);
                    }
                }

                Button {
                    id: resetButton
                    text: qsTr("Reset")
                    Layout.margins: 2
                    Layout.fillWidth: true
                    enabled: false
                    onClicked: {
                        resultsOverlay.graphics.clear();
                        solveButton.enabled = true;
                        resetButton.enabled = false;
                    }
                }
            }
        }
    }

    PictureMarkerSymbol {
        id: facilitySymbol
        url: "https://static.arcgis.com/images/Symbols/SafetyHealth/FireStation.png"
        height: 30
        width: 30
    }

    PictureMarkerSymbol {
        id: incidentSymbol
        url: "https://static.arcgis.com/images/Symbols/SafetyHealth/esriCrimeMarker_56_Gradient.png"
        height: 30
        width: 30
    }

    SimpleLineSymbol {
        id: routeSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "blue"
        width: 2.0
    }

    ClosestFacilityTask {
        id: task
        url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ClosestFacility"

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            task.createDefaultParameters();
        }

        onCreateDefaultParametersStatusChanged: {
            if (createDefaultParametersStatus !== Enums.TaskStatusCompleted)
                return;

            busy = false;
            solveButton.enabled = true;
            facilityParams = createDefaultParametersResult;
            facilityParams.setFacilitiesWithFeatureTable(facilitiesFeatureTable, params);
            facilityParams.setIncidentsWithFeatureTable(incidentsFeatureTable, params);
        }

        onSolveClosestFacilityStatusChanged: {
            if (solveClosestFacilityStatus !== Enums.TaskStatusCompleted)
                return;

            // finding the closest facility for each incident to create a route graphic between each pair
            for (let incidentIndex = 0; incidentIndex < incidentsFeatureTable.numberOfFeaturesAsInt; incidentIndex++) {
                const closestFacilityIndex = solveClosestFacilityResult.rankedFacilityIndexes(incidentIndex)[0];
                const route = solveClosestFacilityResult.route(closestFacilityIndex, incidentIndex);
                const routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", { geometry: route.routeGeometry, symbol: routeSymbol});

                resultsOverlay.graphics.append(routeGraphic);
            }
            busy = false;
            resetButton.enabled = true;
        }
        onErrorChanged: console.log(error.message);
    }

    QueryParameters {
        id: params
        whereClause: "1=1"
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: busy
    }

    function setViewpointGeometry() {
        // proceed only if both layers are loaded
        if ((facilitiesLayer.loadStatus !== Enums.LoadStatusLoaded) || (incidentsLayer.loadStatus !== Enums.LoadStatusLoaded))
            return;

        // return if set viewpoint has been executed already
        if (mySetViewpointTaskId)
            return;

        mySetViewpointTaskId = mapView.setViewpointGeometryAndPadding(GeometryEngine.unionOf(facilitiesLayer.fullExtent, incidentsLayer.fullExtent), 20);
        task.load();
    }
}
