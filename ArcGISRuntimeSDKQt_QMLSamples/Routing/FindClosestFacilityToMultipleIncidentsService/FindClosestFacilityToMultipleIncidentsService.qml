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
import Esri.ArcGISRuntime 100.6
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

            // declare feature layer for facilities
            FeatureLayer {
                id: facilitiesLayer
                // declare feature table to load the facilities table
                ServiceFeatureTable {
                    id: facilitiesFeatureTable
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Facilities/FeatureServer/0"
                }
                // apply the facilities symbol as a renderer for this feature layer
                renderer: SimpleRenderer {
                    symbol: facilitySymbol
                }
                // set viewpoint geometry
                onLoadStatusChanged: setViewpointGeometry();
            }

            // declare feature layer for incidents
            FeatureLayer {
                id: incidentsLayer
                // declare feature table to load the incidents table
                ServiceFeatureTable {
                    id: incidentsFeatureTable
                    url: "https://services2.arcgis.com/ZQgQTuoyBrtmoGdP/ArcGIS/rest/services/San_Diego_Incidents/FeatureServer/0"
                }
                // appy the incidents symbol as a renderer for this feature layer
                renderer: SimpleRenderer {
                    symbol: incidentSymbol
                }
                // set viewpoint geometry
                onLoadStatusChanged: setViewpointGeometry();
            }
            Component.onCompleted: busy = true;
        }

        // declare graphics overlay for the route graphics
        GraphicsOverlay {
            id: resultsOverlay
        }

        // background for buttons
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

            // column layout for solve and reset buttons
            ColumnLayout {
                Button {
                    id: solveButton
                    text: qsTr("Solve Routes")
                    Layout.margins: 2
                    Layout.fillWidth: true
                    enabled: false
                    onClicked: {
                        // enable busy indicator
                        busy = true;
                        // disable solve button
                        solveButton.enabled = false;
                        // execute solve closest facility task
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
                        // clear graphics overlay
                        resultsOverlay.graphics.clear();
                        // enable solve button
                        solveButton.enabled = true;
                        // disable reset button
                        resetButton.enabled = false;
                    }
                }
            }
        }
    }

    // declare picture marker symbol for the facilities feature layer
    PictureMarkerSymbol {
        id: facilitySymbol
        url: "https://static.arcgis.com/images/Symbols/SafetyHealth/FireStation.png"
        height: 30
        width: 30
    }

    // declare picture marker symbol for the incidents feature layer
    PictureMarkerSymbol {
        id: incidentSymbol
        url: "https://static.arcgis.com/images/Symbols/SafetyHealth/esriCrimeMarker_56_Gradient.png"
        height: 30
        width: 30
    }

    // declare simple line symbol for the route graphics
    SimpleLineSymbol {
        id: routeSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "blue"
        width: 2.0
    }

    // declare closest facility task
    ClosestFacilityTask {
        id: task
        url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/ClosestFacility"

        // once loaded created default parameters
        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            task.createDefaultParameters();
        }

        onCreateDefaultParametersStatusChanged: {
            if (createDefaultParametersStatus !== Enums.TaskStatusCompleted)
                return;

            // disable busy indicator
            busy = false;
            // enable solve button
            solveButton.enabled = true;
            // set facilityParams to closest facilities parmeters
            facilityParams = createDefaultParametersResult;
            // set the facilities parameter with the facilitites feature table
            facilityParams.setFacilitiesWithFeatureTable(facilitiesFeatureTable, params);
            // set the incidents parameter with the incidents feature table
            facilityParams.setIncidentsWithFeatureTable(incidentsFeatureTable, params);
        }

        onSolveClosestFacilityStatusChanged: {
            if (solveClosestFacilityStatus !== Enums.TaskStatusCompleted)
                return;

            // loop through all incidnets
            for (var incidentIndex = 0; incidentIndex < incidentsFeatureTable.numberOfFeaturesAsInt; incidentIndex++) {
                // get the index of the facility closest to the incident
                var closestFacilityIndex = solveClosestFacilityResult.rankedFacilityIndexes(incidentIndex)[0];
                // get the route between the incident and the facility
                var route = solveClosestFacilityResult.route(closestFacilityIndex, incidentIndex)
                // create a graphic from the routes geometry using the route symbol
                var routeGraphic = ArcGISRuntimeEnvironment.createObject
                        ("Graphic", { geometry: route.routeGeometry, symbol: routeSymbol});
                // append graphic to graphics overlay
                resultsOverlay.graphics.append(routeGraphic);
            }
            // disable busy indicator
            busy = false;
            // enable reset button
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
        // return if either or both layers are not loaded
        if ((facilitiesLayer.loadStatus !== Enums.LoadStatusLoaded) || (incidentsLayer.loadStatus !== Enums.LoadStatusLoaded))
            return;

        // return if set viewpoint has been executed already
        if (mySetViewpointTaskId)
            return;

        // set viewpoint geometry from the combined extents of both feature layers
        mySetViewpointTaskId = mapView.setViewpointGeometryAndPadding(GeometryEngine.unionOf(facilitiesLayer.fullExtent, incidentsLayer.fullExtent), 20);
        // load the closest facility task
        task.load();
    }
}
