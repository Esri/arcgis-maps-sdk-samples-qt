// [WriteFile Name=PerformValveIsolationTrace, Category=UtilityNetwork]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url featureServiceUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleGas/FeatureServer"
    readonly property string domainNetworkName: "Pipeline"
    readonly property string tierName: "Pipe Distribution System"
    readonly property string networkSourceName: "Gas Device"
    readonly property string assetGroupName: "Meter"
    readonly property string assetTypeName: "Customer"
    readonly property string globalId: "98A06E95-70BE-43E7-91B7-E34C9D3CB9FF"
    property var traceConfiguration: null
    property var startingLocation: null
    property var categories: null
    property bool uiEnabled: false

    MapView {
        id: mapView
        anchors.fill: parent

        GraphicsOverlay {
            id: startingLocationOverlay
            Graphic {
                id: startingLocationGraphic
                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleCross
                    color: "lime"
                    size: 25
                }
            }
        }

        Map {
            id: map
            BasemapStreetsNightVector {}

            Component.onCompleted: {
                utilityNetwork.load();
            }

            FeatureLayer {
                id: distributionLineLayer
                ServiceFeatureTable {
                    url: featureServiceUrl + "/3"
                }
            }

            FeatureLayer {
                id: deviceLayer
                ServiceFeatureTable {
                    url: featureServiceUrl + "/0"
                }
            }

            UtilityCategoryComparison {
                id: categoryComparison
            }

            UtilityTraceFilter {
                id: traceFilter
            }

            UtilityTraceParameters {
                id: traceParameters
            }

            QueryParameters {
                id: queryParameters
            }

            UtilityNetwork {
                id: utilityNetwork
                url: featureServiceUrl

                onTraceStatusChanged: {
                    if (traceStatus !== Enums.TaskStatusCompleted)
                        return;

                    // re-enable UI and select features
                    uiEnabled = true;

                    if (traceResult.count < 1) {
                        messageDialog.visible = true;
                        return;
                    }

                    let utilityTraceResult = traceResult.get(0);
                    let allElements = traceResult.get(0).elements;

                    // if no elements found, then display message
                    if (allElements.length === 0) {
                        messageDialog.visible = true;
                        return;
                    }

                    // iterate through the map's features
                    for (let i = 0; i < map.operationalLayers.count; i++) {
                        let currentFeatureLayer = map.operationalLayers.get(i);

                        // create query parameters to find features whose network names match the layer's feature table name
                        let objectIds = [];
                        for (let j = 0; j < allElements.length; j++) {
                            let networkSourceName = allElements[j].networkSource.name;
                            let featureTableName = currentFeatureLayer.featureTable.tableName;
                            if (networkSourceName === featureTableName) {
                                objectIds.push(allElements[j].objectId);
                            }
                        }

                        queryParameters.objectIdsAsInts = objectIds;
                        currentFeatureLayer.selectFeaturesWithQuery(queryParameters, Enums.SelectionModeNew);
                    }
                }

                onFeaturesForElementsStatusChanged: {
                    if (featuresForElementsStatus !== Enums.TaskStatusCompleted)
                        return;

                    // display starting location
                    if (featuresForElementsResult.count > 0) {
                        let startingLocationGeometry = featuresForElementsResult.get(0).geometry;
                        startingLocationGraphic.geometry = startingLocationGeometry;
                        mapView.setViewpointCenterAndScale(startingLocationGeometry, 3000);

                        uiEnabled = true;
                    }
                }

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    // get a trace configuration from a tier
                    if (definition.domainNetwork(domainNetworkName) !== null) {
                        let domainNetwork = definition.domainNetwork(domainNetworkName);
                        if (domainNetwork.tier(tierName) !== null) {
                            let tier = domainNetwork.tier(tierName);
                            traceConfiguration = tier.traceConfiguration;
                        }
                    }

                    if (traceConfiguration === null)
                        return;

                    // set the trace filter
                    traceConfiguration.filter = traceFilter;

                    // get a default starting location
                    if (definition.networkSource(networkSourceName) !== null) {
                        let networkSource = definition.networkSource(networkSourceName);
                        if (networkSource.assetGroup(assetGroupName) !== null) {
                            let assetGroup = networkSource.assetGroup(assetGroupName);
                            if (assetGroup.assetType(assetTypeName) !== null) {
                                let assetType = assetGroup.assetType(assetTypeName);
                                startingLocation = createElementWithAssetType(assetType, globalId);
                            }
                        }
                    }

                    if (startingLocation === null)
                        return;

                    // display starting location
                    featuresForElements([startingLocation]);

                    // populate the combo box choices
                    let allCategories = definition.categories;
                    categories = [];
                    for (let i = 0; i < allCategories.length; i++) {
                        categories.push(allCategories[i].name);
                    }

                    comboBox.model = categories;
                }
            }

        }

        ColumnLayout {
            anchors {
                left: parent.left
                top: parent.top
            }
            Rectangle {
                id: backgroundRect
                color: "#FBFBFB"
                height: childrenRect.height
                width: row.width * 1.5

                RowLayout {
                    id: titleRow
                    Rectangle {
                        color: backgroundRect.color
                        width: childrenRect.width
                        height: childrenRect.height
                        Text {
                            text: "Choose category for filter barrier:"
                            font.pixelSize: 14
                        }
                    }
                    anchors {
                        left: row.left
                        top: parent.top
                    }
                }

                RowLayout {
                    id: row
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: titleRow.bottom
                    }
                    ComboBox {
                        id: comboBox
                        enabled: uiEnabled
                        Layout.minimumWidth: 200
                        model: categories
                    }
                    Button {
                        text: "Trace"
                        onClicked: {
                            if (comboBox.currentIndex < 0)
                                return;

                            // disable UI and perform trace
                            uiEnabled = false;

                            // clear previous selection from the feature layers
                            for (let i = 0; i < map.operationalLayers.count; i++) {
                                map.operationalLayers.get(i).clearSelection();
                            }

                            let categoriesList = utilityNetwork.definition.categories;

                            // get the selected utility category
                            let selectedCategory = categoriesList[comboBox.currentIndex];
                            categoryComparison.category = selectedCategory;
                            categoryComparison.comparisonOperator = Enums.UtilityCategoryComparisonOperatorExists;

                            // set the category comparison to the barriers of the configuration's trace filter
                            traceConfiguration.filter.barriers = categoryComparison;

                            // set whether to include isolated features
                            traceConfiguration.includeIsolatedFeatures = checkBox.checked;

                            // build parameters for the isolation trace
                            traceParameters.traceType = Enums.UtilityTraceTypeIsolation;
                            traceParameters.startingLocations = [startingLocation];
                            traceParameters.traceConfiguration = traceConfiguration;

                            utilityNetwork.trace(traceParameters);
                        }
                        enabled: uiEnabled
                    }
                }

                RowLayout {
                    id: checkBoxRow
                    anchors {
                        top: row.bottom
                        left: row.left
                    }
                    CheckBox {
                        id: checkBox
                        text: "Include isolated features"
                        enabled: uiEnabled
                        leftPadding: 0
                    }
                }
            }
        }

        BusyIndicator {
            id: busyIndicator
            anchors.centerIn: parent
            running: utilityNetwork.traceStatus === Enums.TaskStatusInProgress
        }
    }

    MessageDialog {
        id: messageDialog
        title: "Perform valve isolation trace"
        text: "Isolation trace returned no elements."
        visible: false
        onRejected: {
            visible = false;
        }
    }
}
