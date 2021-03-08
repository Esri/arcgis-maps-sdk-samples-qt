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
import Esri.ArcGISRuntime 100.11

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    property var symbolNames: []

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISNavigation
            }

            // Set the scale at which feature symbols and text will appear at their default size
            referenceScale: 100000

            FeatureLayer {
                id: webLayer
                ServiceFeatureTable {
                    url: "http://services.arcgis.com/V6ZHFr6zdgNZuVG0/arcgis/rest/services/LA_County_Points_of_Interest/FeatureServer/0"
                }

                UniqueValueRenderer {
                    id: uniqueValueRenderer

                    // The UniqueValueRenderer will affect specific features based on the values of the specified FieldName(s)
                    fieldNames: ["cat2"]
                }

                Component.onCompleted: {
                    const symbolKeys = ["atm", "beach", "campground", "city-hall", "hospital", "library", "park", "place-of-worship", "police-station", "post-office", "school", "trail"];
                    let symbolsFetchedCount = 0;

                    symbolKeys.forEach((symbolKey) => {
                                           var symbolStyle = ArcGISRuntimeEnvironment.createObject("SymbolStyle", {styleName: "Esri2DPointSymbolsStyle", portal: {}});

                                           symbolStyle.fetchSymbolStatusChanged.connect(() =>
                                                                                        {
                                                                                            if (symbolStyle.fetchSymbolStatus !== Enums.TaskStatusCompleted)
                                                                                            return;

                                                                                            // If multiple field names are set, we can pass multiple values from each field,
                                                                                            // However, even though we are using the same symbol, we must create a UniqueValue for each value from the same field
                                                                                            // When the FeatureLayer is rendered, all features with a matching value in the specified FieldNames will appear with the defined UniqueValue
                                                                                            getValuesFromSymbolLabel(symbolKey).forEach((value) => {
                                                                                                                                            // The resulting legend will use the order of UniqueValues in the UniqueValueRenderer, so we ensure that it is kept in alphabetical order
                                                                                                                                            addToRendererAndSort(ArcGISRuntimeEnvironment.createObject("UniqueValue", {label: symbolKey, values: [value], symbol: symbolStyle.fetchSymbolResult}));
                                                                                                                                        });

                                                                                            symbolsFetchedCount++;

                                                                                            // Populate the legend from the UniqueValuesRenderer's UniqueValuesList once all UniqueValues have been added
                                                                                            // The underlying signal will only trigger once, so we need to ensure it triggers after all UniqueValues have been added
                                                                                            if (symbolsFetchedCount >= symbolKeys.length) {
                                                                                                map.autoFetchLegendInfos = true;
                                                                                            }
                                                                                        });

                                           symbolStyle.fetchSymbolWithKeyList([symbolKey]);
                                       });
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
        onMapScaleChanged: {
            // Set scale symbols to true when we zoom in so the symbols don't take up the entire view
            webLayer.scaleSymbols = (mapView.mapScale >= 80000);
        }
    }

    Rectangle {
        id: legendRect
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
                model: map.legendInfos

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

                    // The LegendInfoListModel populates from all values in a layer's UniqueValueRenderer
                    // Therefore we hide duplicate values with this following codeblock
                    Component.onCompleted: {
                        if (symbolNames.includes(symbolText.text)) {
                            legendItem.height = 0;
                            legendItem.visible = false;
                        } else {
                            symbolNames.push(symbolText.text)
                        }
                    }
                }
            }
        }
    }

    function addToRendererAndSort(uniqueValue) {
        uniqueValueRenderer.uniqueValues.append(uniqueValue);
        let idx = uniqueValueRenderer.uniqueValues.count - 1;

        while (true) {
            if (idx < 1 || uniqueValueRenderer.uniqueValues.get(idx).label >= uniqueValueRenderer.uniqueValues.get(idx-1).label) {
                break;
            }
            uniqueValueRenderer.uniqueValues.move(idx, idx-1);
            idx--;
        }
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
