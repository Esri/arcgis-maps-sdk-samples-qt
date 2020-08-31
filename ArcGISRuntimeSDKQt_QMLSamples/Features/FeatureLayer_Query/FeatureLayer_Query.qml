// [WriteFile Name=FeatureLayer_Query, Category=Features]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600

    // Map view UI presentation at top
    MapView {
        id: mapView

        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            id: map

            BasemapTopographic {}
            initialViewpoint: viewPoint

            FeatureLayer {
                id: featureLayer
                maxScale: 10000

                // default property (renderer)
                SimpleRenderer {
                    SimpleFillSymbol {
                        style: Enums.SimpleFillSymbolStyleSolid
                        color: Qt.rgba(1, 1, 0, 0.6)

                        // default property (outline)
                        SimpleLineSymbol {
                            style: Enums.SimpleLineSymbolStyleSolid
                            color: "black"
                            width: 2.0
                            antiAlias: true
                        }
                    }
                }

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://services.arcgis.com/jIL9msH9OI208GCb/arcgis/rest/services/USA_Daytime_Population_2016/FeatureServer/0"
                }
            }
        }

        // initial viewPoint
        ViewpointCenter {
            id: viewPoint
            center: Point {
                x: -11e6
                y: 5e6
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
            targetScale: 9e7
        }

        Row {
            id: findRow

            anchors {
                top: parent.top
                bottom: map.top
                left: parent.left
                right: parent.right
                margins: 5
            }
            spacing: 5

            TextField {
                id: findText
                width: parent.width * 0.25
                placeholderText: "Enter a state name to select"
                inputMethodHints: Qt.ImhNoPredictiveText
                selectByMouse: true
                validator: RegExpValidator{ regExp: /^[a-zA-Z ]*$/ }
                Keys.onReturnPressed: {
                    btn.clicked();
                }
            }

            Button {
                id: btn
                text: "Find and Select"
                enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
                onClicked: {
                    queryFeatures(findText.text, featureTable) // query the table
                    .then(result=>selectResults(result, featureLayer)) // then select the features
                    .then(features=>mapView.setViewpointGeometryAndPadding(features[0].geometry, 30)); // then zoom to the select feature
                }
            }
        }

        Dialog {
            id: errorMsgDialog
            modal: true
            x: Math.round(parent.width - width) / 2
            y: Math.round(parent.height - height) / 2
            standardButtons: Dialog.Ok
            property alias text : textLabel.text
            Text {
                id: textLabel
                text: "No state named " + findText.text.toUpperCase() + " exists."
            }
        }
    }

    function selectResults(result, layer) {
        return new Promise(
                    (resolve, reject)=>{
                        // clear any previous selection
                        layer.clearSelection();

                        // get the features
                        const features = []
                        while (result.iterator.hasNext) {
                            features.push(result.iterator.next());
                        }

                        // select the features
                        // The ideal way to select features is to call featureLayer.selectFeaturesWithQuery(), which will
                        // automatically select the features based on your query.  This is just a way to show you operations
                        // that you can do with query results. Refer to API doc for more details.
                        layer.selectFeatures(features);

                        resolve(features)
                    });
    }


    function queryFeatures(searchString, table){
        return new Promise(
                    (resolve, reject)=>{
                        let taskId;
                        let parameters = ArcGISRuntimeEnvironment.createObject("QueryParameters");
                        parameters.whereClause = "STATE_NAME LIKE '" + formatStateNameForQuery(findText.text) + "%'";

                        const featureStatusChanged = ()=>{
                            switch (table.queryFeaturesStatus){
                                case Enums.TaskStatusCompleted:
                                table.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                                const result = table.queryFeaturesResults[taskId]
                                if (result) {
                                    resolve(result)
                                }
                                else {
                                    reject({message: "The query finished but there was no result for this taskId", taskId: taskId});
                                }
                                break;
                                case Enums.TaskStatusErrored:
                                table.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                                if (table.error) reject(table.error)
                                else reject({message: table.tableName + ": query task errored++++"})
                                break;
                            }
                        }

                        table.queryFeaturesStatusChanged.connect(featureStatusChanged);
                        taskId = table.queryFeatures(parameters);
                    })
    }

    function formatStateNameForQuery(stateName) {
        // format state names as expected by the service, for instance "Rhode Island"
        if (stateName === "")
            return "";

        const formattedWords = [];

        const lowerStateName = stateName.toLowerCase();
        const words = lowerStateName.split(" ");
        words.forEach(word => formattedWords.push(word.charAt(0).toUpperCase() + word.slice(1)));

        return formattedWords.join(" ");
    }
}
