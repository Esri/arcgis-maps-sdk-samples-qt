// [WriteFile Name=DeleteFeaturesFeatureService, Category=EditData]
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
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

Rectangle {
    width: 800
    height: 600

    property Point calloutLocation
    property string damageType
    

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            // Set the initial basemap to Streets
            BasemapStreets { }

            // Set the initial viewpoint over The United States
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
                            console.log("successfully deleted feature");
                        }
                    }

                    // signal handler for the asynchronous deleteFeature method
                    onDeleteFeatureStatusChanged: {
                        if (deleteFeatureStatus === Enums.TaskStatusCompleted) {
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

                            // delete the feature in the feature table asynchronously
                            featureTable.deleteFeature(feat);
                        }
                    }
                }

                // signal handler for selecting features
                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        if (!selectFeaturesResult.iterator.hasNext)
                            return;

                        var feat  = selectFeaturesResult.iterator.next();
                        damageType = feat.attributes.attributeValue("typdamage");
                        calloutLocation = feat.geometry.extent.center;

                        // show the callout
                        callout.showCallout();
                    }
                }
            }
        }

        QueryParameters {
            id: params
        }

        // hide the callout after navigation
        onViewpointChanged: {
            if (callout.calloutVisible)
                callout.dismiss();
        }

        onMouseClicked: {
            // reset the map callout and update window
            featureLayer.clearSelection();
            if (callout.calloutVisible)
                callout.dismiss();

            //! [DeleteFeaturesFeatureService identify feature]
            // call identify on the feature layer
            var tolerance = 10;
            var returnPopupsOnly = false;
            mapView.identifyLayerWithMaxResults(featureLayer, mouse.x, mouse.y, tolerance, returnPopupsOnly, 1);
            //! [DeleteFeaturesFeatureService identify feature]
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

        calloutData {
            location: calloutLocation
            // We use the HTML to bold, increase the pt size, and center-align the the damageType title.
            title: "<br><b><font size=\"+2\">%1</font></b>".arg(damageType)
        }

        // map callout window
        Callout {
            id: callout
            accessoryButtonType: "Custom"
            customImageUrl: "qrc:/Samples/EditData/DeleteFeaturesFeatureService/ic_menu_trash_light.png"
            calloutData: parent.calloutData
            borderWidth: 1
            borderColor: "lightgrey"
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                if (callout.calloutVisible)
                    callout.dismiss();
                featureLayer.selectedFeatures();
            }
        }
    }
}
