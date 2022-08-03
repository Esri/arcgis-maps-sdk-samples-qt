// [WriteFile Name=QueryOGCAPICQLFilters, Category=Layers]
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
import QtQuick 2.12
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.14

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            FeatureLayer {
                OgcFeatureCollectionTable {
                    id: ogcFeatureCollectionTable
                    url: "https://demo.ldproxy.net/daraa"
                    collectionId: "TransportationGroundCrv"

                    // Enums.FeatureRequestModeManualCache specifies that features from the server will be stored locally for display and querying
                    // In this mode, ServiceFeatureTable::populateFromService() must be called to populate the local cache
                    featureRequestMode: Enums.FeatureRequestModeManualCache

                    onLoadStatusChanged: {
                        // ogcFeatureCollectionTable.load() will be automatically called when added to a FeatureLayer
                        // populateFromService() will be called with the initial viewpoint extent
                        if (ogcFeatureCollectionTable.loadStatus === Enums.LoadStatusLoaded)
                            ogcFeatureCollectionTable.populateFromService(queryParameters, false, []);
                    }
                }

                renderer: SimpleRenderer {
                    SimpleLineSymbol {
                        style: Enums.SimpleLineSymbolStyleSolid
                        color: "darkMagenta"
                        width: 3
                    }
                }
            }

            // initial viewPoint
            ViewpointCenter {
                targetScale: 200000
                center: Point {
                    x: 36.10
                    y: 32.62
                    spatialReference: SpatialReference {
                        wkid: 4326
                    }
                }
            }
        }

        QueryParameters {
            id: queryParameters

            // Set the default value for max features returned
            maxFeatures: 1000

            // Set the default where clause
            whereClause: "F_CODE = 'AP010'"
        }

        Rectangle {
            anchors {
                fill: controlColumn
                margins: -5
            }
            color: "#efefef"
            radius: 5
            border {
                color: "darkgray"
                width: 1
            }
        }

        Column {
            id: controlColumn
            anchors {
                left: parent.left
                top: parent.top
                margins: 10
            }
            spacing: 5

            Row {
                spacing: 5
                Text {
                    id: whereClauseText
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Where Clause"
                }

                ComboBox {
                    id: whereClauseMenu
                    width: 200
                    model: ["F_CODE = 'AP010'", "{ \"op\": \"=\", \"args\": [ { \"property\": \"F_CODE\" }, \"AP010\" ] }", "F_CODE LIKE 'AQ%'", "{\"op\": \"and\", \"args\":[{ \"op\": \"=\", \"args\":[{ \"property\" : \"F_CODE\" }, \"AP010\"]}, { \"op\": \"t_before\", \"args\":[{ \"property\" : \"ZI001_SDV\"},\"2013-01-01\"]}]}"]
                }
            }

            Row {
                spacing: 8
                Text {
                    id: maxFeatureText
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Max Features"
                }

                TextField {
                    id: maxFeatureField
                    anchors.verticalCenter: parent.verticalCenter
                    width: 200
                    text: "1000"
                    selectByMouse: true
                    validator: IntValidator {}
                }
            }

            Row {
                spacing: 8
                Text {
                    id: fromField
                    anchors.verticalCenter: parent.verticalCenter
                    text: "From"
                    rightPadding: 40
                }

                TextField {
                    id: fromDate
                    anchors.verticalCenter: parent.verticalCenter
                    width: 200
                    text: ""
                    selectByMouse: true
                    validator: RegExpValidator {
                        regExp: /(0[1-9]|1[012])[/](0[1-9]|[12][0-9]|3[01])[/](19|20)\d\d/
                    }
                    placeholderText: "MM/DD/YYYY"
                }
            }

            Row {
                spacing: 8
                Text {
                    id: toField
                    anchors.verticalCenter: parent.verticalCenter
                    text: "To"
                    rightPadding: 53
                }

                TextField {
                    id: toDate
                    anchors.verticalCenter: parent.verticalCenter
                    width: 200
                    text: ""
                    selectByMouse: true
                    validator: RegExpValidator {
                        regExp: /(0[1-9]|1[012])[/](0[1-9]|[12][0-9]|3[01])[/](19|20)\d\d/
                    }
                    placeholderText: "MM/DD/YYYY"
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Query"


                onClicked: {
                    if (!ogcFeatureCollectionTable)
                        return;

                    queryParameters.whereClause = whereClauseMenu.currentText
                    queryParameters.geometry = mapView.currentViewpointExtent.extent
                    queryParameters.maxFeatures = maxFeatureField.text

                    if (fromDate.text && toDate.text) {
                        const startDate = new Date(Date.fromLocaleDateString(Qt.locale(), fromDate.text, "MM/dd/yyyy"));
                        const endDate = new Date(Date.fromLocaleDateString(Qt.locale(), toDate.text, "MM/dd/yyyy"));

                        const newTimeExtent = ArcGISRuntimeEnvironment.createObject("TimeExtent", {
                                                                                        "startTime": startDate,
                                                                                        "endTime": endDate
                                                                                    });

                        queryParameters.timeExtent = newTimeExtent;
                    }

                    // Populate the feature collection table with features that match the parameters,
                    // clear the cache to prepare for the new query results, and store all table fields
                    ogcFeatureCollectionTable.populateFromService(queryParameters, true, []);
                }
            }
        }
    }
}
