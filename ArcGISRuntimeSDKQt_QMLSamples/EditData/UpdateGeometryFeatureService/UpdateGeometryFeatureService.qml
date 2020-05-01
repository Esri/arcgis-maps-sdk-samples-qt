// [WriteFile Name=UpdateGeometryFeatureService, Category=EditData]
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
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600
    
    property bool featureSelected: false
    property Point newLocation
    property var selectedFeature: null

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            ViewpointCenter {
                Point {
                    x: -10800000
                    y: 4500000
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
                targetScale: 3e7
            }

            FeatureLayer {
                id: featureLayer

                // declare as child of feature layer, as featureTable is the default property
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"

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

                function doUpdateAttribute(){
                    if (selectedFeature.loadStatus === Enums.LoadStatusLoaded) {
                        selectedFeature.onLoadStatusChanged.disconnect(doUpdateAttribute);

                        // set the geometry
                        selectedFeature.geometry = newLocation;
                        // update the feature in the feature table asynchronously
                        featureTable.updateFeature(selectedFeature);

                        featureSelected = false;
                        selectedFeature = null;
                        featureLayer.clearSelection();
                    }
                }

                // signal handler for asynchronously fetching the selected feature
                onSelectedFeaturesStatusChanged: {
                    if (selectedFeaturesStatus === Enums.TaskStatusCompleted) {
                        while (selectedFeaturesResult.iterator.hasNext) {
                            // obtain the feature
                            selectedFeature = selectedFeaturesResult.iterator.next();

                            selectedFeature.onLoadStatusChanged.connect(doUpdateAttribute);
                            selectedFeature.load();
                        }
                    }
                }

                // signal handler for selecting features
                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        if (!selectFeaturesResult.iterator.hasNext)
                            featureSelected = false;
                        else
                            featureSelected = true;
                    }
                }
            }
        }

        QueryParameters {
            id: params
        }

        onMouseClicked: {
            // if a feature is selected, move it to a new location
            if (featureSelected) {
                // obtain the new point to move the feature to
                newLocation = mouse.mapPoint;
                // asynchronously fetch the selected feature
                featureLayer.selectedFeatures();
            } else {
                // call identify on the mapview
                mapView.identifyLayerWithMaxResults(featureLayer, mouse.x, mouse.y, 10, false, 1);
            }
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                if (identifyLayerResult.geoElements.length > 0) {
                    // get the objectid of the identifed object
                    params.objectIdsAsInts = [identifyLayerResult.geoElements[0].attributes.attributeValue("objectid")];
                    // query for the feature using the objectid
                    featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
                }
            }
        }
    }
}
