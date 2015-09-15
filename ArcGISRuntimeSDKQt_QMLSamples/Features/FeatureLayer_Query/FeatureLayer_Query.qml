// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Map view UI presentation at top
    MapView {
        id: mv

        anchors.fill: parent

        Map {
            BasemapTopographic {}
            initialViewpoint: vc

            FeatureLayer {
                id: featureLayer

                // default property (simple renderer)
                SimpleRenderer {
                    SimpleFillSymbol {
                        style: Enums.SimpleFillSymbolStyleSolid
                        color: "yellow"
                        opacity: 0.5

                        // default property (outline)
                        SimpleLineSymbol {
                            style: Enums.SimpleLineSymbolStyleSolid
                            color: "black"
                            width: 2.0 * scaleFactor
                            antiAlias: true
                            opacity: 0.6
                        }
                    }
                }

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/USA/MapServer/2"

                    onQueryFeaturesStatusChanged: {
                        if (queryFeaturesStatus === Enums.TaskStatusCompleted) {
                            // clear selected features if any
                            featureLayer.clearSelection();

                            var featureResults = [];
                            while (queryFeaturesResult.iterator.hasNext) {
                                featureResults.push(queryFeaturesResult.iterator.next());
                            }

                            if (featureResults.length > 0) {
                                featureLayer.selectFeatures(featureResults);

                                // zoom to the first feature
                                mv.setViewpointGeometryAndPadding(featureResults[0].geometry, 200);
                            }
                            else {
                                errorMsgDialog.visible = true;
                            }
                        }
                    }
                }
            }
        }

        QueryParameters {
            id: params
            outFields: ["*"]
        }

        Row {
           id: findRow

           anchors {
               top: parent.top
               bottom: mv.top
               left: parent.left
               right: parent.right
               margins: 5
           }
           spacing: 5

           TextField {
               id: findText
               width: parent.width * 0.25
               placeholderText: "Enter a state name to select"
           }
           Button {
               text: "Find"
               enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
               onClicked: {
                   // set the where clause
                   params.whereClause = "STATE_NAME LIKE \'" + findText.text.toUpperCase() + "\'";

                   // start the query
                   featureTable.queryFeatures(params);
               }
           }
       }

        MessageDialog {
            id: errorMsgDialog
            visible: false
            text: "No state like " + findText.text.toUpperCase() + " exists."
            onAccepted: {
                visible = false;
            }
        }

        ViewpointCenter {
            id: vc
            center: Point {
                x: -11e6
                y: 5e6
                spatialReference: SpatialReference {
                    wkid: 102113
                }
            }
            scale: 9e7
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
