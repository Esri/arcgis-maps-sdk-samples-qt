// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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
import Esri.ArcGISRuntime 100.15

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    property var legendItems: []

    SymbolStyle {
        id: symbolStyle
        styleName: "Esri2DPointSymbolsStyle"

        SymbolStyleSearchParameters {
            id: symbolStyleSearchParameters
            keys: ["atm", "beach", "campground", "city-hall", "hospital", "library", "park", "place-of-worship", "police-station", "post-office", "school", "trail"]
            keysStrictlyMatch: true
        }

        Component.onCompleted: {
            symbolStyle.searchSymbolsStatusChanged.connect(searchSymbolsHandler);
            symbolStyle.searchSymbols(symbolStyleSearchParameters);
        }

        function searchSymbolsHandler() {
            if (symbolStyle.searchSymbolsStatus !== Enums.TaskStatusCompleted)
                return;

            legendItems = symbolStyle.searchSymbolsResult;
            symbolStyle.searchSymbolsResult.forEach((symbolResult) => {
                                                        symbolResult.fetchSymbolStatusChanged.connect(() => fetchSymbolsHandler(symbolResult));
                                                        symbolResult.fetchSymbol();
                                                    });
        }

        function fetchSymbolsHandler(symbolResult) {
            if (symbolResult.fetchSymbolStatus !== Enums.TaskStatusCompleted)
                return;

            const values = getValuesFromSymbolLabel(symbolResult.key);
            values.forEach((value) => {
                               const uniqueValue = ArcGISRuntimeEnvironment.createObject("UniqueValue", {
                                                                                             label: symbolResult.key,
                                                                                             values: [value],
                                                                                             symbol: symbolResult.fetchSymbolResult
                                                                                         }, webLayer);
                               webLayerUniqueValueRenderer.uniqueValues.append(uniqueValue);
                           });
        }

        function getValuesFromSymbolLabel(symbolLabel) {
            switch (symbolLabel) {
            case "atm": return ["Banking and Finance"];
            case "beach": return ["Beaches and Marinas"];
            case "campground": return ["Campgrounds"];
            case "city-hall": return ["City Halls", "Government Offices"];
            case "hospital": return ["Hospitals and Medical Centers", "Health Screening and Testing", "Health Centers", "Mental Health Centers"];
            case "library": return ["Libraries"];
            case "park": return ["Parks and Gardens"];
            case "place-of-worship": return ["Churches"];
            case "police-station": return ["Sheriff and Police Stations"];
            case "post-office": return ["DHL Locations", "Federal Express Locations"];
            case "school": return ["Public High Schools", "Public Elementary Schools", "Private and Charter Schools"];
            case "trail": return ["Trails"];
            default: return [];
            }
        }
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISNavigation
            }

            // Set the scale at which feature symbols and text will appear at their default size
            referenceScale: 100000

            FeatureLayer {
                id: webLayer
                // Set scale symbols to true when we zoom in so the symbols don't take up the entire view
                scaleSymbols: mapView.mapScale >= 80000

                ServiceFeatureTable {
                    url: "https://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0"
                }

                UniqueValueRenderer {
                    id: webLayerUniqueValueRenderer

                    // The UniqueValueRenderer will affect specific features based on the values of the specified FieldName(s)
                    fieldNames: ["cat2"]
                }
            }

            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -118.44186
                    y: 34.28301
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 7000
            }
        }
    }

    Rectangle {
        id: legendRectangle
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 545
        width: 175
        color: "lightgrey"
        opacity: 0.9
        clip: true
        border {
            color: "darkgrey"
            width: 1
        }

        // Catch mouse signals so they don't propagate to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Column {
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 2

            Row {
                spacing: 55

                Text {
                    text: qsTr("Legend")
                    font {
                        pixelSize: 18
                        bold: true
                    }
                }
            }

            // Create a list view to display the Legend
            ListView {
                id: legendListView
                anchors.margins: 10
                width: parent.width * .9
                height: parent.height * .9
                clip: true
                model: legendItems

                // Create delegate to display the name with an image
                delegate: Item {
                    id: legendItem
                    width: 175
                    height: 40
                    clip: true

                    Row {
                        spacing: 5
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 25
                            height: width
                            source: symbolUrl
                        }

                        Text {
                            id: symbolText
                            anchors.verticalCenter: parent.verticalCenter
                            width: 110
                            text: name
                            wrapMode: Text.Wrap
                            font.pixelSize: 12
                        }
                    }
                }
            }

        }
    }
}
