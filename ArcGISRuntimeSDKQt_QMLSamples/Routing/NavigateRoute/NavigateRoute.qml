// [WriteFile Name=NavigateRoute, Category=Routing]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.8
import QtQuick.Layouts 1.11
import QtPositioning 5.2

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url routeTaskUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"
    property var m_route: null
    property var m_routeResult: null
//    property var routeTracker: null
    property var directionListModel: null

    MapView {
        id: mapView
        anchors.fill: parent

        locationDisplay.onLocationChanged: {
            /*
    Point projectedLocation = GeometryEngine::project(location.position(), SpatialReference::wgs84());
    QGeoPositionInfo tempLocation(QGeoCoordinate(projectedLocation.y(), projectedLocation.x()), QDateTime::currentDateTime());
    m_routeTracker->trackLocation(tempLocation);
*/
            console.log(typeof(mapView.locationDisplay.location));
//            var projectedLocation = GeometryEngine.project(mapView.locationDisplay.location, SpatialReference.createWgs84());
//            QtPositioning.coordinate(projectedLocation.y(), projectedLocation.x());
//            new Date();
//            var tempLocation = new QGeoPositionInfo(QtPositioning.coordinate(projectedLocation.y(), projectedLocation.x()), new Date());
//            routeTracker.trackLocation(tempLocation);
//            var tempLocation = new QGeoPositionInfo(new QGeoCoordinate(projectedLocation.y(), projectedLocation.x()), QDateTime.currentDateTime());
//            routeTracker.trackLocation(mapView.locationDisplay.location);
        }

        Map {
            BasemapNavigationVector {}

        }

        GraphicsOverlay {
            id: routeOverlay
            Graphic {
                id: routeAheadGraphic
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDash
                    color: "blue"
                    width: 5
                }
            }
            Graphic {
                id: routeTraveledGraphic
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "cyan"
                    width: 3
                }
            }
            Graphic {
                Point {
                    id: conventionCenterPoint
                    x: -117.160386727
                    y: 32.706608
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }
            Graphic {
                Point {
                    id: memorialPoint
                    x: -117.173034
                    y: 32.712327
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }
            Graphic {
                Point {
                    id: aerospaceMuseumPoint
                    x: -117.147230
                    y: 32.730467
                    SpatialReference {wkid: 4326}
                }
                symbol: stopSymbol
            }
        }

        SimpleMarkerSymbol {
            id: stopSymbol
            style: "SimpleMarkerSymbolStyleDiamond"
            color: "red"
            size: 20
        }

        Stop {
            id: stop1
            geometry: conventionCenterPoint
        }
        Stop {
            id: stop2
            geometry: memorialPoint
        }
        Stop {
            id: stop3
            geometry: aerospaceMuseumPoint
        }

        RouteTask {
            id: routeTask
            url: routeTaskUrl
            Component.onCompleted: {
                load();
            }
            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    createDefaultParameters();
                }
            }
            onCreateDefaultParametersStatusChanged: {
                if (createDefaultParametersStatus === Enums.TaskStatusCompleted) {
                    createDefaultParametersResult.returnStops = true;
                    createDefaultParametersResult.returnDirections = true;
                    createDefaultParametersResult.returnRoutes = true;
                    createDefaultParametersResult.outputSpatialReference = SpatialReference.createWgs84();
                    createDefaultParametersResult.setStops([stop1, stop2, stop3]);

                    //solve the route with these parameters
                    routeTask.solveRoute(createDefaultParametersResult);
                }
            }
            onSolveRouteStatusChanged: {
                if (solveRouteStatus === Enums.TaskStatusCompleted) {
                    if (solveRouteResult.routes.length > 0) {
                        m_routeResult = solveRouteResult;
                        m_route = solveRouteResult.routes[0];
                        mapView.setViewpointGeometryAndPadding(m_route.routeGeometry, 100);
                        routeAheadGraphic.geometry = m_route.routeGeometry;
                        navigateButton.enabled = true;
                    }
                }
            }
        }

        Rectangle {
            id: backBox
            z: 1
            width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 200 : 300
            height: 200
            color: "#FBFBFB"
            border.color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 20

            RowLayout {
                id: buttonRow
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    margins: 5
                }
                Button {
                    id: navigateButton
                    text: "Navigate"
                    enabled: false
                    onClicked: {
                        startNavigation();
                        enabled = false;
                    }

                }
                Button {
                    id: recenterButton
                    text: "Recenter"
                    enabled: false
//                    enabled: model.recenterButtonEnabled

                }
            }

            Rectangle {
                anchors {
                    top: buttonRow.bottom
                    left: parent.left
                    margins: 5
                }
                width: parent.width
                Text {
                    padding: 5
                    width: parent.width
                    wrapMode: Text.Wrap
//                    text:
                }
            }
        }
        SimulatedLocationDataSource {
            id: simulatedLocationDataSource
        }

        RouteTracker {
            id: routeTracker

            Component.onCompleted: {
                console.log("route tracker created");
            }

            onTrackingStatusResultChanged: {

                console.log(trackingStatusResult.routeProgress.remainingDistance.displayText);
            }

            onTrackLocationStatusChanged: {
//                console.log("track location status changed");
                if (trackLocationStatus === Enums.TaskStatusInProgress) {
                    console.log("in progress");
                } else if (trackLocationStatus === Enums.TaskStatusReady) {
                    console.log("ready");
                } else if (trackLocationStatus === Enums.TaskStatusCompleted) {
                    console.log("completed");
                } else if (trackLocationStatus === Enums.TaskStatusErrored) {
                    console.log("errored");
                }
            }
        }
    }
    function startNavigation() {
        console.log("pressed");

        // get the directions for the route
        directionListModel = m_route.directionManeuvers;

        // set properties for route tracker
        routeTracker.routeResult = m_routeResult;
        routeTracker.routeIndex = 0;

        // turn on mapview's navigation mode
        mapView.locationDisplay.autoPanMode = Enums.LocationDisplayAutoPanModeNavigation;

        // add a data source for the location display
        simulatedLocationDataSource.setLocationsWithPolyline(m_route.routeGeometry);
        mapView.locationDisplay.dataSource = simulatedLocationDataSource;
        simulatedLocationDataSource.start();
    }
}
