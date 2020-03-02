// [WriteFile Name=OfflineRouting, Category=Routing]
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
import QtQuick.Controls 2.6
import Esri.ArcGISRuntime 100.8
import Esri.ArcGISExtras 1.1
import QtQml 2.11

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/tpk/"

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            minScale: 100000
            Basemap {
                ArcGISTiledLayer {
                    TileCache {
                        path: dataPath + "san_diego/streetmap_SD.tpk"
                    }
                }
            }
        }

        GraphicsOverlay {
            id: stopsOverlay
        }

        GraphicsOverlay {
            id: routeOverlay
            renderer: SimpleRenderer {
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "blue"
                    width: 2
                }
            }
        }

        GraphicsOverlay {
            id: boundaryOverlay
            Graphic {
                Envelope {
                    xMin: -13045352.223196
                    xMax: -13024588.857198
                    yMin: 3864910.900750
                    yMax: 3838880.505604
                    SpatialReference {wkid: 3857}
                }
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleDash
                    color: "lime"
                    width: 3
                }
            }
        }

        RouteTask {
            id: routeTask
            url: dataPath + "san_diego/sandiego.geodatabase"
            networkName: "Streets_ND"

            //            RouteParameters {
            //                id: routeParameters
            //            }

            onComponentCompleted: {
                load();
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                {
                    createDefaultParameters();
                }
            }

            onCreateDefaultParametersStatusChanged: {
                if (createDefaultParametersStatus === Enums.TaskStatusCompleted) {
                    var routeParameters = createDefaultParametersResult;

                    findRoute = function() {
                        let stopsList = [];
                        let numGraphics = stopsOverlay.graphics.count;
                        if (numGraphics > 1) {
                            for (let i = 0; i < numGraphics; i++) {
                                let tempStop = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: stopsOverlay.graphics.get(i).geometry});
                                stopsList.push(tempStop);
                            }
                            routeParameters.setStops(stopsList);
                            routeParameters.travelMode = routeTask.routeTaskInfo.travelModes[0];

                            routeTask.solveRoute(routeParameters);
                        }
                    }
                }
            }

            onErrorChanged: {
                console.log(routeTask.error.message);
                console.log(routeTask.error.additionalMessage);
            }

            onSolveRouteStatusChanged: {
                if (solveRouteStatus === Enums.TaskStatusCompleted)
                {
                    // clear old route
                    routeOverlay.graphics.clear();
                    if (routeTask.error) {
                    }
                    var routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: solveRouteResult.routes[0].routeGeometry});
                    routeOverlay.graphics.append(routeGraphic);
                }
            }
        }

        //        onMousePressed: {
        //            mapView.identifyGraphicsOverlayWithMaxResults(stopsOverlay, mouse.x, mouse.y, 10, false, 1);
        //        }

        onMouseClicked: {
            var clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
            let numStops = stopsOverlay.graphics.count + 1;
            var stopLabel = ArcGISRuntimeEnvironment.createObject("TextSymbol", {
                                                                      color: "red",
                                                                      size: 30,
                                                                      text: numStops
                                                                  });
            var stopGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: stopLabel});
            stopsOverlay.graphics.append(stopGraphic);
            findRoute();
        }
    }


    BusyIndicator {
        anchors.centerIn: parent
        visible: true
        running: routeTask.taskStatus === Enums.TaskStatusInProgress
    }

    property var findRoute;
}
