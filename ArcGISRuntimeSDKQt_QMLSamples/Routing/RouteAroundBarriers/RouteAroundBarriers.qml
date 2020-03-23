// [WriteFile Name=RouteAroundBarriers, Category=Routing]
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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.8
import QtQuick.Layouts 1.11

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property int checkBoxPadding: 20
    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/symbol"
    readonly property url routeTaskUrl: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"
    property var stopsList: []
    property var barriersList: []
    property var createAndDisplayRoute
    property var routeParameters: null
    property var directionListModel: null
    property bool addStops: false
    property bool addBarriers: false
    property bool findBestSeq: false
    property bool preserveFirstStp: false
    property bool preserveLastStp: false

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapStreets {}

            ViewpointCenter {
                Point {
                    x: -117.1750
                    y: 32.727
                    SpatialReference {
                        wkid: 4326
                    }
                }
                targetScale: 40000
            }
        }

        GraphicsOverlay {
            id: stopsOverlay
        }

        PictureMarkerSymbol {
            id: pinSymbol
            url: dataPath + "/orange_symbol.png"
        }

        GraphicsOverlay {
            id: routeOverlay
            renderer: SimpleRenderer {
                SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "blue"
                    width: 3
                }
            }
        }

        GraphicsOverlay {
            id: barriersOverlay
        }

        SimpleFillSymbol {
            id: barrierSymbol
            style: Enums.SimpleFillSymbolStyleDiagonalCross
            color: "red"
            outline: SimpleLineSymbol {
                style: Enums.SimpleLineSymbolStyleNull
            }
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
                    routeParameters = createDefaultParametersResult;
                    routeParameters.returnStops = true;
                    routeParameters.returnDirections = true;
                }
            }

            function createAndDisplayRoute() {
                if (stopsList.length > 1) {
                    routeOverlay.graphics.clear();

                    routeParameters.setStops(stopsList);
                    routeParameters.setPolygonBarriers(barriersList);
                    routeParameters.findBestSequence = findBestSeq;
                    routeParameters.preserveFirstStop = preserveFirstStp;
                    routeParameters.preserveLastStop = preserveLastStp;
                    solveRoute(routeParameters);
                }
            }

            onSolveRouteStatusChanged: {
                if (solveRouteStatus === Enums.TaskStatusCompleted) {
                    if (solveRouteResult.routes.length === 0) {
                        console.log("No routes found");
                        return;
                    }

                    //                    m_directions = route.directionManeuvers(this);
                    //                    emit directionsChanged();

                    let route = solveRouteResult.routes[0];
                    let routeGeometry = route.routeGeometry;
                    let routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: routeGeometry});
                    routeOverlay.graphics.append(routeGraphic);

                    directionListModel = route.directionManeuvers;
                }
            }
        }

        onMouseClicked: {
            let clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);

            if (addStops) {
                let stopPoint = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: clickedPoint});
                stopsList.push(stopPoint);

                let textSymbol = ArcGISRuntimeEnvironment.createObject("TextSymbol", {text: stopsList.length, color: "white", size: 16, horizontalAlignment: Enums.HorizontalAlignmentCenter, verticalAlignment: Enums.VerticalAlignmentBottom});
                textSymbol.offsetY = pinSymbol.height/2;

                let stopSymbol = ArcGISRuntimeEnvironment.createObject("CompositeSymbol");
                stopSymbol.symbols.append(pinSymbol);
                stopSymbol.symbols.append(textSymbol);
                let stopGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: stopSymbol});
                stopsOverlay.graphics.append(stopGraphic);

                routeTask.createAndDisplayRoute();
            } else if (addBarriers) {
                let barrierPolygon = GeometryEngine.buffer(clickedPoint, 500);
                let barrier = ArcGISRuntimeEnvironment.createObject("PolygonBarrier", {geometry: barrierPolygon});
                barriersList.push(barrier);

                let barrierGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: barrierPolygon, symbol: barrierSymbol});
                barriersOverlay.graphics.append(barrierGraphic);

                routeTask.createAndDisplayRoute();
            }

        }

        ColumnLayout {
            spacing: 0
            Layout.alignment: Qt.AlignTop

            Rectangle {
                id: backBox
                z: 1
                Layout.alignment: Qt.AlignLeft
                Layout.margins: 3
                Layout.bottomMargin: 0
                width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 200 : 300
                height: childrenRect.height
                color: "lightgrey"

                GridLayout {
                    id: grid
                    rows: 3
                    columns: 1
                    rowSpacing: 10
                    columnSpacing: 2
                    Layout.alignment: Qt.AlignHCenter
//                    anchors.centerIn: parent

                    Row {
                        id: buttonsRow
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 5
                        padding: 5
                        Button {
                            id: stopButton
                            text: "Add stops"
                            onClicked: {
                                highlighted = !highlighted;
                                addStops = highlighted;
                            }
                        }
                        Button {
                            id: barrierButton
                            text: "Add barriers"

                            onClicked: {
                                highlighted = !highlighted;
                                addBarriers = highlighted;

                                stopButton.highlighted = false;
                                addStops = false;
                            }
                        }
                    }

                    Column {
                        spacing: 2

                        CheckBox {
                            id: bestSequenceBox
                            text: "Find best sequence"
                            onCheckedChanged: {
                                findBestSeq = checked;
                                routeTask.createAndDisplayRoute();
                            }
                        }
                        CheckBox {
                            id: firstStopBox
                            text: "Preserve first stop"
                            leftPadding: checkBoxPadding
                            enabled: bestSequenceBox.checked
                            onCheckedChanged: {
                                preserveFirstStp = checked;
                                routeTask.createAndDisplayRoute();
                            }
                        }
                        CheckBox {
                            id: lastStopBox
                            text: "Preserve last stop"
                            leftPadding: checkBoxPadding
                            enabled: bestSequenceBox.checked
                            onCheckedChanged: {
                                preserveLastStp = checked;
                                routeTask.createAndDisplayRoute();
                            }
                        }
                    }

                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Button {
                            id: resetButton
                            text: "Reset"
                            onClicked: {
                                // clear stops
                                routeParameters.clearStops();
                                stopsList = [];

                                // clear barriers
                                routeParameters.clearPolygonBarriers();
                                barriersList = [];

                                // clear directions

                                // clear graphics overlays
                                routeOverlay.graphics.clear();
                                stopsOverlay.graphics.clear();
                                barriersOverlay.graphics.clear();
                            }
                        }
                    }
                }
            }

            // Create window for displaying the route directions
            Rectangle {
                id: directionWindow
                Layout.alignment: Qt.AlignBottom
                Layout.topMargin: 0
                visible: true
                Layout.preferredWidth: backBox.width
                Layout.preferredHeight: 300
                Layout.margins: 3
                color: "lightgrey"

                //! [RouteAroundBarriers cpp ListView directionsView]
                ListView {
                    id: directionsView
                    anchors {
                        fill: parent
                        margins: 5
                    }
                    header: Component {
                        Text {
                            height: 40
                            text: "Directions:"
                            font.pixelSize: 22
                        }
                    }

                    // set the model to the DirectionManeuverListModel returned from the route
                    model: directionListModel
                    delegate: directionDelegate
                }
                //! [RouteAroundBarriers cpp ListView directionsView]
            }

        }
    }

    Component {
        id: directionDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35
            color: directionWindow.color

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8
                    leftMargin: 20
                    rightMargin: 20
                }
                color: "darkgrey"
                height: 1
            }

            Text {
                text: directionText
                anchors {
                    fill: parent
                    leftMargin: 5
                }
                elide: Text.ElideRight
                font.pixelSize: 14
            }
        }
    }
}
