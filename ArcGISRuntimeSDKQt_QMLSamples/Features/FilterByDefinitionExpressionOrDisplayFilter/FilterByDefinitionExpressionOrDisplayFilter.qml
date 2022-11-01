// [WriteFile Name=FeatureLayer_DefinitionExpression, Category=Features]
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

import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime

//! [Rectangle-mapview-map-viewpoint]
Rectangle {
    width: 800
    height: 600



    // Map view UI presentation at top
    MapView {
        id: mv
        anchors.fill: parent
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
            initialViewpoint: viewPoint

            FeatureLayer {
                id: featureLayer

                ServiceFeatureTable {
                    id: featureTable
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SF311/FeatureServer/0"

                    // Initalize the feature count when the feature layer first loads
                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded)
                            queryFeatureCountInCurrentExtent();
                    }
                }
            }
        }

        ViewpointCenter {
            id: viewPoint
            center: Point {
                x: -13630484
                y: 4545415
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
            targetScale: 300000
        }

        onDrawStatusChanged: {

            // Recalculate the feature count in current extent any time the map redraws
            if (drawStatus === Enums.DrawStatusCompleted)
            {
                queryFeatureCountInCurrentExtent();
            }
        }
    }
    //! [Rectangle-mapview-map-viewpoint]

    Column {
        id: expressionRow
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 5
            bottomMargin: 25
        }
        spacing: 5

        Label {
            text: "Current feature count: " + featureTable.queryFeatureCountResult;
        }
        // button to apply a definition expression
        Button {
            text: "Apply Expression"
            width: 200
            enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
            onClicked: {
                featureLayer.definitionExpression = "req_Type = \'Tree Maintenance or Damage\'"
            }
        }
        // button to apply display filter
        Button {
            text: "Apply Display Filter"
            width: 200
            enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
            onClicked: {
                const displayFilter = ArcGISRuntimeEnvironment.createObject("DisplayFilter", {name: "Damaged Trees", whereClause: "req_Type = \'Tree Maintenance or Damage\'"});
                const displayFilterDefintionVar = ArcGISRuntimeEnvironment.createObject("ManualDisplayFilterDefinition");
                displayFilterDefintionVar.availableFilters.append(displayFilter);
                displayFilterDefintionVar.activeFilter = displayFilter;

                featureLayer.displayFilterDefinition = displayFilterDefintionVar;

                queryFeatureCountInCurrentExtent();
            }
        }

        // button to reset the definition expression
        Button {
            text: "Reset"
            width: 200
            enabled: featureTable.loadStatus === Enums.LoadStatusLoaded
            onClicked: {
                featureLayer.definitionExpression = "";

                const displayFilter = ArcGISRuntimeEnvironment.createObject("DisplayFilter", {name: "No Filter", whereClause: "1=1"});
                const displayFilterDefintionVar = ArcGISRuntimeEnvironment.createObject("ManualDisplayFilterDefinition");
                displayFilterDefintionVar.availableFilters.append(displayFilter);
                displayFilterDefintionVar.activeFilter = displayFilter;
                featureLayer.displayFilterDefinition = displayFilterDefintionVar;

                queryFeatureCountInCurrentExtent();
            }
        }
    }

    function queryFeatureCountInCurrentExtent() {
        const queryParams = ArcGISRuntimeEnvironment.createObject(
                              "QueryParameters", {
                                  "geometry": mv.currentViewpointExtent.extent
                              });

        featureTable.queryFeatureCount(queryParams);
    }
}
