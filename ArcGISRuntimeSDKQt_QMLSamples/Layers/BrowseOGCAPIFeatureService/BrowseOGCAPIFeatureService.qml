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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.ArcGISRuntime
import QtQuick.Dialogs

Rectangle {
    id: root
    clip: true
    width: 800
    height: 600

    property url serviceURL: "https://demo.ldproxy.net/daraa"
    property OgcFeatureService featureService: null
    property FeatureLayer featureLayer : null
    property string errorMessage: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic

                // When the map has loaded, connect to the OGC Feature Service
                onComponentCompleted: loadFeatureService(serviceURL);
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
                    enabled: featureService.loadStatus !== Enums.LoadStatusLoading
                    onClicked: {
                        serviceURL = serviceURLBox.text;
                        loadFeatureService(serviceURL);
                    }
                }
                ComboBox {
                    id: featureCollectionListComboBox
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    textRole: "title"
                    model: featureService.loadStatus === Enums.LoadStatusLoaded ? root.featureService.serviceInfo.featureCollectionInfos : [];
                }
                Button {
                    id: loadLayerButton
                    text: "Load selected layer"
                    enabled: featureLayer.loadStatus !== Enums.LoadStatusLoading
                    onClicked: loadFeatureCollection(featureCollectionListComboBox.currentIndex);
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
            }
        }

        // Pop-up error message box
        MessageDialog {
            id: errorMessageBox
            title: "Error message!"
            text: errorMessage
            icon: StandardIcon.Warning
            visible: errorMessage === "" ? false : true;
            onAccepted: errorMessage = "";
        }

        QueryParameters {
            id: queryParameters
            maxFeatures: 1000
        }
    }

    function loadFeatureService(currentUrl) {
        // Create feature service object and assign to featureService property
        root.featureService = ArcGISRuntimeEnvironment.createObject("OgcFeatureService", {url: currentUrl}, map);

        // Connect loaded signal to checkForLoadingErrors() function
        root.featureService.loadStatusChanged.connect(checkForLoadingErrors);

        featureService.load();
    }

    function handleError(error) {
        if (!error.additionalMessage)
            errorMessage = error.message;
        else
            errorMessage = error.message + "\n" + error.additionalMessage;
    }

    function checkForLoadingErrors() {
        if (root.featureService.loadStatus === Enums.LoadStatusFailedToLoad) {
            handleError(root.featureService.loadError);
        }
    }

    function loadFeatureCollection(selectedFeature) {
        // Create featureCollectionTable object
        let featureCollectionTable = ArcGISRuntimeEnvironment.createObject("OgcFeatureCollectionTable");

        // Set request mode to manual cache
        featureCollectionTable.featureRequestMode = Enums.FeatureRequestModeManualCache;

        // Copy info for selected feature collection into featureCollectionTable
        featureCollectionTable.featureCollectionInfo = featureService.serviceInfo.featureCollectionInfos[selectedFeature];

        // Populate featureCollectionTable
        featureCollectionTable.populateFromService(queryParameters, true, []);

        // Create feature layer object and assign to featureLayer property
        root.featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", { featureTable: featureCollectionTable } );

        // Connect loadStatusChanged to checkIfLayerLoaded function
        root.featureLayer.loadStatusChanged.connect(checkIfLayerLoaded);

        root.featureLayer.load();
    }

    function checkIfLayerLoaded() {
        if (root.featureLayer.loadStatus === Enums.LoadStatusFailedToLoad) {
            handleError(root.featureLayer.loadError);
            return;
        }
        else if (root.featureLayer.loadStatus !== Enums.LoadStatusLoaded) {
            return;
        }
        addFeatureLayerToMap();
    }

    function addFeatureLayerToMap() {
        map.operationalLayers.clear();

        // Add selected feature layer to the map
        map.operationalLayers.append(root.featureLayer);

        // Set the viewpoint of the map
        mapView.setViewpointGeometry(root.featureLayer.fullExtent);
    }
}
