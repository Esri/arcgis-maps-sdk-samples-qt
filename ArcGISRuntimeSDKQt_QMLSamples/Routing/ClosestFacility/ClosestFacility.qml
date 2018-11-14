// [WriteFile Name=ClosestFacilitySample, Category=Routing]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property bool busy: false
    property string message: ""
    property var facilities: []
    property var facilityParams: null

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent

        Map {
            BasemapStreets {}

            initialViewpoint: ViewpointCenter {
                Point {
                    x: -13041154
                    y: 3858170
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 1e5
            }

            onLoadStatusChanged: {
                createFacilities();
                task.load();
            }
        }

        GraphicsOverlay {
            id: facilitiesOverlay

            SimpleRenderer {
                PictureMarkerSymbol {
                    url: "https://static.arcgis.com/images/Symbols/SafetyHealth/Hospital.png"
                    height: 30
                    width: 30
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3042129900625112E7
                    y: 3860127.9479775648
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3042129900625112E7
                    y: 3860127.9479775648
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3042193400557665E7
                    y: 3862448.873041752
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3046882875518233E7
                    y: 3862704.9896770366
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3040539754780494E7
                    y: 3862924.5938606677
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3042571225655518E7
                    y: 3858981.773018156
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3039784633928463E7
                    y: 3856692.5980474586
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }

            Graphic {
                geometry: Point {
                    x: -1.3049023883956768E7
                    y: 3861993.789732541
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        GraphicsOverlay {
            id: resultsOverlay
        }

        onMouseClicked: {
            if (busy === true)
                return;

            if (facilityParams === null)
                return;

            resultsOverlay.graphics.clear();

            var incidentGraphic = ArcGISRuntimeEnvironment.createObject(
                "Graphic", {geometry: mouse.mapPoint, symbol: incidentSymbol});
            resultsOverlay.graphics.append(incidentGraphic);

            solveRoute(mouse.mapPoint);
        }
    }

    SimpleMarkerSymbol {
        id: incidentSymbol
        style: Enums.SimpleMarkerSymbolStyleCross
        color: "black"
        size: 20
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

            setupRouting();
        }

        onCreateDefaultParametersStatusChanged: {
            if (createDefaultParametersStatus !== Enums.TaskStatusCompleted)
                return;

            busy = false;
            facilityParams = createDefaultParametersResult;
            facilityParams.setFacilities(facilities);
        }

        onSolveClosestFacilityStatusChanged: {
            if (solveClosestFacilityStatus !== Enums.TaskStatusCompleted)
                return;

            busy = false;

            if (solveClosestFacilityResult === null || solveClosestFacilityResult.error)
                message = "Incident not within San Diego Area!";

            var rankedList = solveClosestFacilityResult.rankedFacilityIndexes(0);
            var closestFacilityIdx = rankedList[0];

            var incidentIndex = 0; // 0 since there is just 1 incident at a time
            var route = solveClosestFacilityResult.route(closestFacilityIdx, incidentIndex);

            var routeGraphic = ArcGISRuntimeEnvironment.createObject(
                "Graphic", { geometry: route.routeGeometry, symbol: routeSymbol});
            resultsOverlay.graphics.append(routeGraphic);
        }

        onErrorChanged: message = error.message;
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: busy
    }

    Dialog {
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Route Error"
        visible: text.length > 0
        property alias text : textLabel.text
        Text {
            id: textLabel
            text: message
        }
    }

    function createFacilities() {
        facilitiesOverlay.graphics.forEach(function(graphic) {
            var facility = ArcGISRuntimeEnvironment.createObject("Facility", {geometry: graphic.geometry});
            facilities.push(facility);
        });
    }


    function setupRouting() {
        busy = true;
        message = "";
        task.createDefaultParameters();
    }

    function solveRoute(incidentPoint) {
        var incident = ArcGISRuntimeEnvironment.createObject("Incident", {geometry: incidentPoint});
        facilityParams.setIncidents( [ incident ] );

        busy = true;
        message = "";
        task.solveClosestFacility(facilityParams);
    }
}
