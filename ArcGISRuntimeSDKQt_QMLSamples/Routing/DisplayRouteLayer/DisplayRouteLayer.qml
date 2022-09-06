// [WriteFile Name=DisplayRouteLayer, Category=Routing]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import Esri.ArcGISRuntime
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property FeatureCollection featureCollection: null
    property string directions: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        PortalItem {
            id: portalItem
            itemId: "0e3c8e86b4544274b45ecb61c9f41336"
            Component.onCompleted: load();
            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded) {
                    return;
                }

                // if the portal item is a feature collection, add the feature collection to the map's operational layers
                if (type === Enums.PortalItemTypeFeatureCollection) {
                    featureCollection = ArcGISRuntimeEnvironment.createObject("FeatureCollection", {item: portalItem});
                    const featureCollectionLayer = ArcGISRuntimeEnvironment.createObject("FeatureCollectionLayer", {featureCollection: featureCollection});

                    featureCollection.onLoadStatusChanged.connect(() => {
                                                                      if (featureCollection.loadStatus === Enums.LoadStatusLoaded) {
                                                                          featureCollection.tables.forEach((table) => {
                                                                                                               if (table.displayName === "DirectionPoints") {
                                                                                                                   if (table.loadStatus === Enums.LoadStatusLoaded) {
                                                                                                                       queryFeatures(table);
                                                                                                                   }
                                                                                                               }
                                                                                                           });
                                                                      }
                                                                  });

                    map.operationalLayers.append(featureCollectionLayer);
                } else {
                    console.warn("Portal item with ID '" + itemId + "' is not a feature collection.")
                }
            }
        }

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic
            initialViewpoint: viewpoint
        }
    }
    ViewpointCenter {
        id: viewpoint
        // Specify the center Point
        center: Point {
            x: -122.8309
            y: 45.2281
            spatialReference: SpatialReference { wkid: 4326 }
        }
        // Specify the scale
        targetScale: 800000
    }
    Button {
        text: "Directions"
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }
        enabled: featureCollection ? featureCollection.loadStatus === Enums.LoadStatusLoaded : false
        onClicked: {
            popup.open();
        }
    }

    Popup {
        id: popup
        anchors.centerIn: Overlay.overlay
        width: 300
        height: 320
        focus: true
        contentItem: ScrollView {
            contentWidth: parent.width - 30
            Text {
                width: parent.width
                text: directions
                wrapMode: Text.WordWrap
            }
            clip: true
        }
        opacity: .9
    }

    function queryFeatures(table){
        return new Promise(
                    (resolve, reject) => {
                        let taskId;
                        let parameters = ArcGISRuntimeEnvironment.createObject("QueryParameters");
                        parameters.whereClause = "1=1";

                        const featureStatusChanged = () => {
                            switch (table.queryFeaturesStatus) {
                                case Enums.TaskStatusCompleted:
                                    table.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                                    const result = table.queryFeaturesResults[taskId];
                                    if (result) {
                                        resolve(result);
                                        var features = Array.from(result.iterator.features);
                                        // Clear the directions list before repopulating it
                                        directions = "";
                                        features.forEach((feature) => {
                                                             directions = directions + "\n - " + feature.attributes.attributeValue("DisplayText");
                                                         });
                                    }
                                    else {
                                        reject({message: "The query finished but there was no result for this taskId", taskId: taskId});
                                    }
                                    break;
                                case Enums.TaskStatusErrored:
                                    table.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                                    if (table.error) {
                                        reject(table.error);
                                    } else {
                                        reject({message: table.tableName + ": query task errored++++"});
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        table.queryFeaturesStatusChanged.connect(featureStatusChanged);
                        taskId = table.queryFeatures(parameters);
                    });
    }
}
