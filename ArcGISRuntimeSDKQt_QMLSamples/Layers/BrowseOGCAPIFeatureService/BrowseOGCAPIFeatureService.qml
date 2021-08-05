// [WriteFile Name=BrowseOGCAPIFeatureService, Category=Layers]
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
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.12

Rectangle {
    id: root
    clip: true
    width: 800
    height: 600

    property url serviceURL: "https://demo.ldproxy.net/daraa"
    property OgcFeatureService featureService: null
    property FeatureLayer featureLayer : null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic

                // When the map has loaded, connect to the OGC Feature Service
                onComponentCompleted: {
                    loadFeatureService(serviceURL);
                }
            }
        }

        // Add the OGC feature service UI element
        Control {
            id: uiControl
            anchors {
                right: parent.right
                top: parent.top
                margins: 10
            }
            padding: 10
            background: Rectangle {
                color: "white"
                border.color: "black"
            }
            contentItem: GridLayout {
                columns: 2
                Label {
                    id: instructionLabel
                    text: "Load the service, then select a layer for display"
                    font.bold: true
                    verticalAlignment: "AlignVCenter"
                    horizontalAlignment: "AlignHCenter"
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
                TextField {
                    id: serviceURLBox
                    text: serviceURL
                    Layout.fillWidth: true
                    selectByMouse: true
                }
                Button {
                    id: connectButton
                    text: "Load service"
                    onClicked: {
                        serviceURL = serviceURLBox.text
                        loadFeatureService(serviceURL)
                    }
                }
                ComboBox {
                    id: featureCollectionListComboBox
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    textRole: "title"
                }
                Button {
                    id: loadLayerButton
                    text: "Load selected layer"
                    onClicked: loadFeatureCollection(featureCollectionListComboBox.currentIndex)
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
            }
        }
    }

    function loadFeatureService(currentUrl) {
        // Create feature service object and assign to featureService property
        root.featureService = ArcGISRuntimeEnvironment.createObject("OgcFeatureService", {url: currentUrl}, map)

        // Connect loaded signal to updateList() function
        root.featureService.loadStatusChanged.connect(checkIfServiceLoaded);

        // Load feature service
        featureService.load();
    }

    function checkIfServiceLoaded() {
        if (root.featureService.loadStatus !== Enums.LoadStatusLoaded) {
            return;
        }
        updateListInInterface();
    }

    function updateListInInterface() {
        // Assign featureCollectionInfos to featureCollectionInfos property
        let featureCollectionInfos = root.featureService.serviceInfo.featureCollectionInfos;

        // Define ComboBox model using list
        featureCollectionListComboBox.model = featureCollectionInfos;
    }

    function loadFeatureCollection(selectedFeature) {
        // Create featureCollectionTable object
        let featureCollectionTable = ArcGISRuntimeEnvironment.createObject("OgcFeatureCollectionTable");

        // Set request mode to manual cache
        featureCollectionTable.featureRequestMode = Enums.FeatureRequestModeManualCache;

        // Copy info for selected feature collection into featureCollectionTable
        featureCollectionTable.featureCollectionInfo = featureService.serviceInfo.featureCollectionInfos[selectedFeature];

        // Create Query Parameters
        var queryParameters = ArcGISRuntimeEnvironment.createObject("QueryParameters", { maxFeatures: 1000 })

        // Populate featureCollectionTable
        featureCollectionTable.populateFromService(queryParameters, true, []);

        // Create feature layer object and assign to featureLayer property
        root.featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", { featureTable: featureCollectionTable } )

        // Connect loadStatusChanged to checkIfLayerLoaded
        root.featureLayer.loadStatusChanged.connect(checkIfLayerLoaded);

        // Load feature layer
        root.featureLayer.load();
    }

    function checkIfLayerLoaded() {
        if (root.featureLayer.loadStatus !== Enums.LoadStatusLoaded) {
            return;
        }
        addFeatureToMap();
    }

    function addFeatureToMap() {
        // Remove all feature layers from the map
        map.operationalLayers.clear()

        // Add selected feature layers to the map
        map.operationalLayers.append(root.featureLayer)

        // Set the viewpoint of the map
        mapView.setViewpointGeometry(root.featureLayer.fullExtent)
    }
}
