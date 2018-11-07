// [WriteFile Name=AddFeaturesFeatureService, Category=EditData]
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
import Esri.ArcGISRuntime 100.5

Rectangle {
    width: 800
    height: 600

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            // set initial viewpoint to The United States
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
                            console.log("successfully added feature");
                        }
                    }

                    // signal handler for the asynchronous addFeature method
                    onAddFeatureStatusChanged: {
                        if (addFeatureStatus === Enums.TaskStatusCompleted) {
                            // apply the edits to the service
                            featureTable.applyEdits();
                        }
                    }
                }
            }
        }

        //! [AddFeaturesFeatureService new feature at mouse click]
        onMouseClicked: {  // mouseClicked came from the MapView
            // create attributes json for the new feature
            var featureAttributes = {"typdamage" : "Minor", "primcause" : "Earthquake"};

            // create a new feature using the mouse's map point
            var feature = featureTable.createFeatureWithAttributes(featureAttributes, mouse.mapPoint);

            // add the new feature to the feature table
            featureTable.addFeature(feature);
        }
        //! [AddFeaturesFeatureService new feature at mouse click]
    }
}
