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
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property bool featureSelected: false
    property Point newLocation

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            // Set the initial viewpoint over Panama
            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -9030446.96
                    y: 943791.32
                    spatialReference: SpatialReference { wkid: 102100 }
                }
                scale: 2e6
            }

            FeatureLayer {
                id: featureLayer

                selectionColor: "cyan"
                selectionWidth: 3 * scaleFactor

                // declare as child of feature layer, as feature table is the default property
                ServiceFeatureTable {
                    id: featureTable
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"

                    // make sure edits are successfully applied to the service
                    onApplyEditsStatusChanged: {
                        if (applyEditsStatus === Enums.TaskStatusCompleted) {
                            console.log("successfully updated feature");
                        }
                    }

                    // signal handler for the asynchronous updateFeature method
                    onUpdateFeatureStatusChanged: {
                        if (updateFeatureStatus === Enums.TaskStatusCompleted) {
                            // apply the edits to the service
                            featureTable.applyEdits();
                        }
                    }
                }

                // signal handler for asynchronously fetching the selected feature
                onSelectedFeaturesStatusChanged: {
                    if (selectedFeaturesStatus === Enums.TaskStatusCompleted) {
                        while (selectedFeaturesResult.iterator.hasNext) {
                            // obtain the feature
                            var feat = selectedFeaturesResult.iterator.next();
                            // set the geometry
                            feat.geometry = newLocation;
                            // update the feature in the table asynchronously
                            featureTable.updateFeature(feat);
                        }
                        featureLayer.clearSelection();
                    }
                }

                // signal handler for selecting features
                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        var count = 0;
                        while (selectFeaturesResult.iterator.hasNext) {
                            selectFeaturesResult.iterator.next();
                            ++count;
                        }
                        if (count > 0)
                            featureSelected = true;
                        else
                            featureSelected = false;
                    }
                }
            }
        }

        QueryParameters {
            id: params
            outFields: ["*"]
            maxFeatures: 1
        }

        onMouseClicked: {
            // if a feature is selected, move it to a new location
            if (featureSelected) {
                // obtain the new point to move the feature to
                newLocation = mouse.mapPoint;
                // asynchronously fetch the selected feature
                featureLayer.selectedFeatures();
                featureSelected = false;
            } else {
                // no feature is selected, so select a new feature
                // create an envelope with some tolerance and query for feature selection within that envelope
                var tolerance = 10 * scaleFactor;
                var mapTolerance = tolerance * unitsPerPixel;
                var envJson = {"xmin" : mouse.mapX - mapTolerance, "ymin" : mouse.mapY - mapTolerance, "xmax" : mouse.mapX + mapTolerance, "ymax" : mouse.mapY + mapTolerance,
                    "spatialReference" : {"wkid": 102100}};
                var envelope = ArcGISRuntimeEnvironment.createObject("Envelope", {"json" : envJson});

                // set the envelope as the geometry for the query parameter
                params.geometry = envelope;
                // query and select the features
                featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
            }
        }
    }

    // neatline
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
