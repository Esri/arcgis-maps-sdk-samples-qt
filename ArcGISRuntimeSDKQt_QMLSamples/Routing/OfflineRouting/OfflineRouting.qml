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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1
import QtQml 2.11
import QtQuick.Layouts 1.11

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/tpk/"
    property var findRoute;
    property Graphic selectedGraphic: null;

    MapView {
        id: mapView
        anchors.fill: parent
        focus: false

        RowLayout {
            anchors {
                left: parent.left
                top: parent.top
            }
            ComboBox {
                id: comboBox
                Layout.fillWidth: true
                onCurrentIndexChanged: {
                    routeTask.findRoute();
                }
            }
            Button {
                text: "Reset"
                Layout.fillWidth: true
                onClicked: {
                    stopsOverlay.graphics.clear();
                    routeOverlay.graphics.clear();
                    selectedGraphic = null;
                }
            }
        }

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
                    id: routableArea
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

        SimpleMarkerSymbol {
            id: stopLabel
            style: Enums.SimpleMarkerSymbolStyleCircle
            color: "red"
            size: 20
        }

        RouteTask {
            id: routeTask
            url: dataPath + "san_diego/sandiego.geodatabase"
            networkName: "Streets_ND"

            onComponentCompleted: {
                load();
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    // assign ComboBox model from travel mode names
                    let modesList = routeTask.routeTaskInfo.travelModes;
                    let travelModeNames = [];
                    for (let i = 0; i < modesList.length; i++) {
                        travelModeNames.push(modesList[i].name);
                    }
                    comboBox.model = travelModeNames;
                    createDefaultParameters();
                }
            }

            function findRoute() {
                if (routeTask.solveRouteStatus === Enums.TaskStatusInProgress)
                    return;

                if (createDefaultParametersResult === null)
                    return;

                let routeParameters = createDefaultParametersResult;
                let stopsList = [];
                let numGraphics = stopsOverlay.graphics.count;
                if (numGraphics > 1) {
                    for (let i = 0; i < numGraphics; i++) {
                        let tempStop = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: stopsOverlay.graphics.get(i).geometry});
                        stopsList.push(tempStop);
                    }
                    routeParameters.setStops(stopsList);
                    routeParameters.travelMode = routeTask.routeTaskInfo.travelModes[comboBox.currentIndex];

                    routeTask.solveRoute(routeParameters);
                }
            }

            onErrorChanged: {
                console.log(routeTask.error.message, routeTask.error.additionalMessage);
            }

            onSolveRouteStatusChanged: {
                if (solveRouteStatus === Enums.TaskStatusCompleted) {
                    // clear old route
                    routeOverlay.graphics.clear();
                    if (routeTask.error) {
                        console.warn(routeTask.error.message);
                    }
                    let routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: solveRouteResult.routes[0].routeGeometry});
                    routeOverlay.graphics.append(routeGraphic);
                }
            }
        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus !== Enums.TaskStatusCompleted)
                return;

            if (identifyGraphicsOverlayResult === null) {
                return;
            }

            if (identifyGraphicsOverlayResult.graphics.length === 0) {
                selectedGraphic = null;
            } else {
                let index = stopsOverlay.graphics.indexOf(identifyGraphicsOverlayResult.graphics[0]);
                selectedGraphic = stopsOverlay.graphics.get(index);
            }
        }

        // check whether mouse pressed over an existing stop
        onMousePressed: {
            mapView.identifyGraphicsOverlay(stopsOverlay, mouse.x, mouse.y, 10, false);
        }

        // get stops from clicked locations and find route
        onMouseClicked: {
            if (!selectedGraphic) {
                let clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
                if (!GeometryEngine.within(clickedPoint, routableArea)) {
                    console.warn("Outside of routable area");
                    return;
                }
                let stopGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: stopLabel});
                stopsOverlay.graphics.append(stopGraphic);
                routeTask.findRoute();
            }
            mouse.accepted = true;
        }

        // if mouse is moved while pressing on a graphic, the click-and-pan effect of the MapView is prevented by mouse.accepted = true
        // and the mouse position is tracked and route is updated on-the-fly
        onMousePositionChanged: {
            mouse.accepted = !!selectedGraphic; // whether to pass mouse event to MapView
            if (selectedGraphic) {
                if (!GeometryEngine.within(mapView.screenToLocation(mouse.x, mouse.y), routableArea)) {
                    console.warn("Outside of routable area");
                    return;
                }
                selectedGraphic.geometry = mapView.screenToLocation(mouse.x, mouse.y);
                routeTask.findRoute();
            }
        }

        // mouseMoved is processed before identifyGraphicsOverlayCompleted, so must clear graphic upon mouseReleased
        onMouseReleased: {
            if (selectedGraphic) {
                selectedGraphic = null;
                mouse.accepted = true;
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: true
        running: routeTask.taskStatus === Enums.TaskStatusInProgress
    }
}
