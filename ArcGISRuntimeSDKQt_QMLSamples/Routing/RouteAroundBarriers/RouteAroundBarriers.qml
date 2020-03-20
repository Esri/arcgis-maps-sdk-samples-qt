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
    property list<Stop> stopsList
    property var createAndDisplayRoute;

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

        RouteTask {
            id: routeTask
            Component.onCompleted: load();

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    createDefaultParameters();
                }
            }

            onCreateDefaultParametersStatusChanged: {
                createDefaultParametersResult.returnStops = true;
                createDefaultParametersResult.returnDirections = true;
            }

            function createAndDisplayRoute() {
                if (stopsList.length > 1) {

                    routeOverlay.graphics.clear();

                    createDefaultParametersResult.setStops(stopsList);
                    solveRoute(createDefaultParametersResult);

//                    m_routeParameters.setStops(m_stopsList);
//                    m_routeParameters.setPolygonBarriers(m_barriersList);
//                    m_routeParameters.setFindBestSequence(m_findBestSequence);
//                    m_routeParameters.setPreserveFirstStop(m_preserveFirstStop);
//                    m_routeParameters.setPreserveLastStop(m_preserveLastStop);

                }
            }
        }

        onMouseClicked: {
            let clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
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
        }

        Rectangle {
            id: backBox
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"
            opacity: 0.8
            radius: 5

            GridLayout {
                id: grid
                rows: 3
                columns: 1
                rowSpacing: 10
                columnSpacing: 2

                Row {
                    id: buttonsRow
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 5
                    padding: 5
                    Button {
                        id: stopButton
                        text: "Stops"
//                        onClicked: {
//                            highlighted = !highlighted;
//                            sampleModel.addStops = highlighted;

//                            // unset barriers button, if set
//                            barrierButton.highlighted = false;
//                            sampleModel.addBarriers = false;
//                        }
                    }
                    Button {
                        id: barrierButton
                        text: "Barriers"
//                        onClicked: {
//                            highlighted = !highlighted;
//                            sampleModel.addBarriers = highlighted;

//                            // unset stops button, if set
//                            stopButton.highlighted = false;
//                            sampleModel.addStops = false;
//                        }
                    }
                    Button {
                        id: resetButton
                        text: "Reset"
//                        onClicked: {
//                            sampleModel.clearRouteAndGraphics();
//                        }
                    }
                }

                Column {
                    spacing: 2

                    CheckBox {
                        id: bestSequenceBox
                        text: "Find best sequence"
//                        onCheckedChanged: {
//                            sampleModel.findBestSequence = checked;
//                            sampleModel.createAndDisplayRoute();
//                        }
                    }
                    CheckBox {
                        id: firstStopBox
                        text: "Preserve first stop"
                        leftPadding: checkBoxPadding
//                        onCheckedChanged: {
//                            sampleModel.preserveFirstStop = checked;
//                            sampleModel.createAndDisplayRoute();
//                        }
                    }
                    CheckBox {
                        id: lastStopBox
                        text: "Preserve last stop"
                        leftPadding: checkBoxPadding
//                        onCheckedChanged: {
//                            sampleModel.preserveLastStop = checked;
//                            sampleModel.createAndDisplayRoute();
//                        }
                    }
                }
            }
        }
    }
}
