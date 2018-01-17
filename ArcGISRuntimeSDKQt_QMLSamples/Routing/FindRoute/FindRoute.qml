// [WriteFile Name=FindRoute, Category=Routing]
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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    id: mainRect
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property Point stop1Geometry: null
    property Point stop2Geometry: null
    property var routeParameters: null
    property var directionListModel: null

    // Create window for displaying the route directions
    Rectangle {
        id: directionWindow
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        visible: false
        width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 250 * scaleFactor : 350 * scaleFactor
        color: "#FBFBFB"

        //! [FindRoute qml ListView directionsView]
        ListView {
            id: directionsView
            anchors {
                fill: parent
                margins: 5 * scaleFactor
            }
            header: Component {
                Text {
                    height: 40 * scaleFactor
                    text: "Directions:"
                    font.pixelSize: 22 * scaleFactor
                }
            }

            // set the model to the DirectionManeuverListModel returned from the route
            model: directionListModel
            delegate: directionDelegate
        }
        //! [FindRoute qml ListView directionsView]
    }

    // Create MapView that contains a Map with the Topographic Basemap
    MapView {
        id: mapView
        anchors.fill: parent

        // set the transform to animate showing the direction window
        transform: Translate {
            id: translate
            x: 0
            Behavior on x { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
        }

        // Create a GraphicsOverlay to display the route
        GraphicsOverlay {
            id: routeGraphicsOverlay

            // Set the renderer
            SimpleRenderer {
                SimpleLineSymbol {
                    color: "cyan"
                    style: Enums.SimpleLineSymbolStyleSolid
                    width: 4
                }
            }
        }

        // Create a GraphicsOverlay to display the stops
        GraphicsOverlay { id: stopsGraphicsOverlay }

        // Create a map with a basemap and initial viewpoint
        Map {
            BasemapNavigationVector {}

            initialViewpoint: ViewpointCenter {
                Point {
                    x: -13041154
                    y: 3858170
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 1e5
            }

            // Add the graphics and setup the RouteTask once the map is loaded
            onLoadStatusChanged: {
                addStopGraphics();
                setupRouteTask();
            }
        }

        // Create the solve button to solve the route
        Rectangle {
            id: solveButton
            property bool pressed: false
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                bottomMargin: 5 * scaleFactor
            }

            width: 130 * scaleFactor
            height: 30 * scaleFactor
            color: pressed ? "#959595" : "#D6D6D6"
            radius: 5
            border {
                color: "#585858"
                width: 1 * scaleFactor
            }

            Text {
                id: routeButtonText
                anchors.centerIn: parent
                text: "Solve route"
                font.pixelSize: 14 * scaleFactor
                color: "#35352E"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: solveButton.pressed = true
                onReleased: solveButton.pressed = false
                onClicked: {
                    if (routeParameters !== null) {
                        // set parameters to return directions
                        routeParameters.returnDirections = true;

                        // clear previous route graphics
                        routeGraphicsOverlay.graphics.clear();

                        // clear previous stops from the parameters
                        routeParameters.clearStops();

                        // set the stops to the parameters
                        var stop1 = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: stop1Geometry, name: "Origin"});
                        var stop2 = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: stop2Geometry, name: "Destination"});
                        routeParameters.setStops([stop1, stop2]);

                        // solve the route with the parameters
                        routeTask.solveRoute(routeParameters);
                    }
                }
            }
        }

        // Create a button to show the direction window
        Rectangle {
            id: directionButton

            property bool pressed: false

            visible: !solveButton.visible
            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 10 * scaleFactor
                bottomMargin: 40 * scaleFactor
            }

            width: 45 * scaleFactor
            height: width
            color: pressed ? "#959595" : "#D6D6D6"
            radius: 100
            border {
                color: "#585858"
                width: 1.5 * scaleFactor
            }

            Image {
                anchors.centerIn: parent
                width: 35 * scaleFactor
                height: width
                source: "qrc:/Samples/Routing/FindRoute/directions.png"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: directionButton.pressed = true
                onReleased: directionButton.pressed = false
                onClicked: {
                    // Show the direction window when it is clicked
                    translate.x = directionWindow.visible ? 0 : (directionWindow.width * -1);
                    directionWindow.visible = !directionWindow.visible;
                }
            }
        }
    }    

    //! [FindRoute RouteTask]

    // Create a RouteTask pointing to an online service
    RouteTask {
        id: routeTask
        url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"

        // Request default parameters once the task is loaded
        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusLoaded) {
                routeTask.createDefaultParameters();
            }
        }

        // Store the resulting route parameters
        onCreateDefaultParametersStatusChanged: {
            if (createDefaultParametersStatus === Enums.TaskStatusCompleted) {
                routeParameters = createDefaultParametersResult;
            }
        }

        // Handle the solveRouteStatusChanged signal
        onSolveRouteStatusChanged: {
            if (solveRouteStatus === Enums.TaskStatusCompleted) {
                // Add the route graphic once the solve completes
                var generatedRoute = solveRouteResult.routes[0];
                var routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: generatedRoute.routeGeometry});
                routeGraphicsOverlay.graphics.append(routeGraphic);

                // set the direction maneuver list model
                directionListModel = generatedRoute.directionManeuvers;

                // hide the solve button and show the direction button
                solveButton.visible = false;
            }
        }
    }
    //! [FindRoute RouteTask]

    Component {
        id: directionDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35 * scaleFactor
            color: directionWindow.color

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8 * scaleFactor
                    leftMargin: 20 * scaleFactor
                    rightMargin: 20 * scaleFactor
                }
                color: "darkgrey"
                height: 1 * scaleFactor
            }

            Text {
                text: directionText
                anchors {
                    fill: parent
                    leftMargin: 5 * scaleFactor
                }
                elide: Text.ElideRight
                font.pixelSize: 14 * scaleFactor
            }
        }
    }

    function addStopGraphics() {
        //! [FindRoute qml addStopGraphics]
        // create the stop graphics' geometry
        stop1Geometry = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                  x: -13041171,
                                                                  y: 3860988,
                                                                  spatialReference: SpatialReference.createWebMercator()
                                                              });
        stop2Geometry = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                  x: -13041693,
                                                                  y: 3856006,
                                                                  spatialReference: SpatialReference.createWebMercator()
                                                              });

        // create the stop graphics' symbols
        var stop1Symbol = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {
                                                                    url: "qrc:/Samples/Routing/FindRoute/pinA.png",
                                                                    width: 32,
                                                                    height: 32,
                                                                    offsetY: 16
                                                                });
        var stop2Symbol = ArcGISRuntimeEnvironment.createObject("PictureMarkerSymbol", {
                                                                    url: "qrc:/Samples/Routing/FindRoute/pinB.png",
                                                                    width: 32,
                                                                    height: 32,
                                                                    offsetY: 16
                                                                });

        // create the stop graphics
        var stop1Graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: stop1Geometry, symbol: stop1Symbol});
        var stop2Graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: stop2Geometry, symbol: stop2Symbol});

        // add to the overlay
        stopsGraphicsOverlay.graphics.append(stop1Graphic);
        stopsGraphicsOverlay.graphics.append(stop2Graphic);
        //! [FindRoute qml addStopGraphics]
    }

    function setupRouteTask() {
        // load the RouteTask
        routeTask.load();
    }
}
