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
import Esri.ArcGISRuntime 100.9
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url pinUrl: "qrc:/Samples/Routing/RouteAroundBarriers/orange_symbol.png"
    readonly property url routeTaskUrl: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/NetworkAnalysis/SanDiego/NAServer/Route"
    readonly property int checkBoxPadding: 20
    property var stopsList: []
    property var barriersList: []
    property var createAndDisplayRoute
    property var routeParameters: null
    property var directionListModel: null
    property var myRoute: null
    property bool addStops: true
    property bool addBarriers: false
    property bool findBestSeq: false
    property bool preserveFirstStp: false
    property bool preserveLastStp: false

    MessageDialog {
        id: messageDialog
        title: "Route around barriers"
        text: "No route found."
        visible: false
        onRejected: {
            visible = false;
        }
    }

    ButtonGroup {
        id: buttonGroup
        buttons: buttonsRow.children
    }

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
            url: pinUrl
            height: 50
            width: 50
            offsetY: height/2
        }

        GraphicsOverlay {
            id: routeOverlay
            SimpleRenderer {
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

                    // set flags to return stops and directions
                    routeParameters.returnStops = true;
                    routeParameters.returnDirections = true;
                }
            }

            function createAndDisplayRoute() {
                if (stopsList.length > 1) {
                    // clear the previous route, if it exists
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
                        messageDialog.visible = true;
                        return;
                    }

                    // get the first route and add to graphics overlay
                    myRoute = solveRouteResult.routes[0];
                    const routeGeometry = myRoute.routeGeometry;
                    const routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: routeGeometry});
                    routeOverlay.graphics.append(routeGraphic);

                    directionListModel = myRoute.directionManeuvers;
                }
            }
        }

        onMouseClicked: {
            const clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);

            if (addStops) {
                // add stop to list of stops
                const stopPoint = ArcGISRuntimeEnvironment.createObject("Stop", {geometry: clickedPoint});
                stopsList.push(stopPoint);

                // create a marker symbol and graphics, and add the graphics to the graphics overlay
                const textSymbol = ArcGISRuntimeEnvironment.createObject("TextSymbol", {
                                                                             text: stopsList.length,
                                                                             color: "white",
                                                                             size: 16,
                                                                             horizontalAlignment: Enums.HorizontalAlignmentCenter,
                                                                             verticalAlignment: Enums.VerticalAlignmentBottom
                                                                         });
                textSymbol.offsetY = pinSymbol.height/2;

                const stopSymbol = ArcGISRuntimeEnvironment.createObject("CompositeSymbol");
                stopSymbol.symbols.append(pinSymbol);
                stopSymbol.symbols.append(textSymbol);
                const stopGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: clickedPoint, symbol: stopSymbol});
                stopsOverlay.graphics.append(stopGraphic);
                routeTask.createAndDisplayRoute();
            } else if (addBarriers) {
                // create barrier and add to list of barriers
                const barrierPolygon = GeometryEngine.buffer(clickedPoint, 500);
                const barrier = ArcGISRuntimeEnvironment.createObject("PolygonBarrier", {geometry: barrierPolygon});
                barriersList.push(barrier);

                const barrierGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: barrierPolygon, symbol: barrierSymbol});
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
                    anchors.horizontalCenter: parent.horizontalCenter

                    Row {
                        id: buttonsRow
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 5
                        padding: 5
                        Button {
                            id: stopButton
                            text: "Add stop"
                            checked: true
                            highlighted: checked
                            onClicked: {
                                checked = true;
                            }
                            onCheckedChanged: {
                                addStops = checked;
                            }
                        }
                        Button {
                            id: barrierButton
                            text: "Add barrier"
                            highlighted: checked
                            onClicked: {
                                checked = true;
                            }
                            onCheckedChanged: {
                                addBarriers = checked;
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
                                directionListModel = [];

                                // clear graphics overlays
                                routeOverlay.graphics.clear();
                                stopsOverlay.graphics.clear();
                                barriersOverlay.graphics.clear();
                            }
                        }
                    }
                    Row {
                        Layout.alignment: Qt.AlignHCenter
                        Button {
                            id: hideShowDirectionsBtn
                            text: "Hide directions"
                            onClicked: {
                                if (text === "Hide directions") {
                                    text = "Show directions";
                                } else {
                                    text = "Hide directions";
                                }
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

                ScrollView {
                    anchors.fill: parent
                    ListView {
                        id: directionsView
                        clip: true
                        visible: hideShowDirectionsBtn.text === "Hide directions" ? true : false
                        anchors {
                            fill: parent
                            margins: 5
                        }
                        header: Text {
                            text: "Directions:"
                            font.pixelSize: 22
                            bottomPadding: 8
                        }

                        // set the model to the DirectionManeuverListModel returned from the route
                        model: directionListModel
                        delegate: directionDelegate
                    }
                }
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
