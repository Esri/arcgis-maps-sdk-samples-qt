// [WriteFile Name=GenerateOfflineMap, Category=Maps]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.5

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    
    property url outputMapPackage: System.temporaryFolder.url + "/OfflineMap_Overrides_%1.mmpk".arg(new Date().getTime().toString())
    property string webMapId: "acc027394bc84c2fb04d1ed317aac674"
    property var generateJob: null
    property alias overrides: offlineMapTask.createGenerateOfflineMapParameterOverridesResult
    property alias parameters: offlineMapTask.createDefaultGenerateOfflineMapParametersResult

    MapView {
        id: mapView
        anchors.fill: parent

        // Create a Map from a Portal Item
        Map {
            id: map

            PortalItem {
                id: mapPortalItem

                itemId: webMapId
                Portal {
                    loginRequired: true
                }
            }
        }

        // Create a button and anchor it to the attribution top
        DownloadButton {
            id: downloadButton
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 5
            }
            visible: map.loadStatus === Enums.LoadStatusLoaded

            onButtonClicked: createDefaultParametersFromRectangle();
        }
    }

    // Create an extent rectangle for selecting the offline area
    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - (50)
        height: parent.height - (125)
        color: "transparent"
        visible: map.loadStatus === Enums.LoadStatusLoaded
        border {
            color: "red"
            width: 3
        }
    }

    function createDefaultParametersFromRectangle() {
        var corner1 = mapView.screenToLocation(extentRectangle.x, extentRectangle.y);
        var corner2 = mapView.screenToLocation((extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
        var envBuilder = ArcGISRuntimeEnvironment.createObject("EnvelopeBuilder");
        envBuilder.setCorners(corner1, corner2);
        var mapExtent = GeometryEngine.project(envBuilder.geometry, SpatialReference.createWebMercator());
        offlineMapTask.createDefaultGenerateOfflineMapParameters(mapExtent);
    }

    // Create Offline Map Task
    OfflineMapTask {
        id: offlineMapTask
        portalItem: mapPortalItem

        onErrorChanged: console.log("error:", error.message, error.additionalMessage);

        onCreateDefaultGenerateOfflineMapParametersStatusChanged: {
            if (createDefaultGenerateOfflineMapParametersStatus !== Enums.TaskStatusCompleted)
                return;

            // Now that we have created our parameters, we can now create out overrides based upon
            // these.
            createGenerateOfflineMapParameterOverrides(createDefaultGenerateOfflineMapParametersResult);
        }

        onCreateGenerateOfflineMapParameterOverridesStatusChanged: {
            console.log("onCreateGenerateOfflineMapParameterOverridesStatusChanged", createGenerateOfflineMapParameterOverridesStatus);
        }


        Component.onDestruction: {
            if (generateJob) {
                generateJob.jobStatusChanged.disconnect(updateJobStatus);
                generateJob.progressChanged.disconnect(updateProgress);
            }
        }
    }

    function overridesReady() {
        return offlineMapTask.createGenerateOfflineMapParameterOverridesStatus === Enums.TaskStatusCompleted;
    }

    function setBasemapLOD(min, max) {
        if (!overridesReady())
            return;

        var layers = map.basemap.baseLayers;
        if (layers && layers.count < 1)
            return;

        // Obtain a key for the given basemap-layer.
        var keyForTiledLayer = ArcGISRuntimeEnvironment.createObject("OfflineMapParametersKey", {initLayer: layers.get(0)});

        if (keyForTiledLayer.empty || keyForTiledLayer.type !== Enums.OfflineMapParametersTypeExportTileCache)
            return;

        // Obtain the dictionary of parameters for taking the basemap offline.
        var dictionary = overrides.exportTileCacheParameters;
        if (!dictionary.contains(keyForTiledLayer))
            return;

        // Create a new sublist of LODs in the range requested by the user.
        var newLODs = [];
        for (var i = min; i < max; ++i )
            newLODs.push(i);

        // Apply the sublist as the LOD level in tilecache parameters for the given
        // service.
        var exportTileCacheParam = dictionary.value(keyForTiledLayer);
        exportTileCacheParam.levelIds = newLODs;
    }

    function setBasemapBuffer(bufferMeters) {
        if (!overridesReady())
            return;

        var layers = map.basemap.baseLayers;
        if (layers && layers.count < 1)
            return;

        // Obtain a key for the given basemap-layer.
        var keyForTiledLayer = ArcGISRuntimeEnvironment.createObject("OfflineMapParametersKey", {initLayer: layers.get(0)});

        if (keyForTiledLayer.empty || keyForTiledLayer.type !== Enums.OfflineMapParametersTypeExportTileCache)
            return;

        // Obtain the dictionary of parameters for taking the basemap offline.
        var dictionary = overrides.exportTileCacheParameters;
        if (!dictionary.contains(keyForTiledLayer))
            return;

        // Create a new geometry around the origional area of interest.
        var bufferGeom = GeometryEngine.buffer(parameters.areaOfInterest, bufferMeters);

        // Apply the geometry to the ExportTileCacheParameters.
        var exportTileCacheParam = dictionary.value(keyForTiledLayer);

        // Set the parameters back into the dictionary.
        exportTileCacheParam.areaOfInterest = bufferGeom;
    }

    function removeSystemValves() {
        removeFeatureLayer("System Valve");
    }


    function getFeatureLayerByName(layerName)
    {
        // Find the feature layer with the given name
        var opLayers = map.operationalLayers;
        for (var i = 0; i < opLayers.count; ++i)
        {
            var candidateLayer = opLayers.get(i);

            if (candidateLayer.layerType === Enums.LayerTypeFeatureLayer && candidateLayer.name.includes(layerName)) {
                return candidateLayer;
            }
        }

        return null;
    }

    function removeFeatureLayer(layerName) {
        if (!overridesReady())
            return;

        var  targetLayer = getFeatureLayerByName(layerName);
        if (!targetLayer)
            return;

        // Obtain a key for the given basemap-layer.
        var keyForTargetLayer = ArcGISRuntimeEnvironment.createObject("OfflineMapParametersKey", {initLayer: targetLayer});

        if (keyForTargetLayer.empty || keyForTargetLayer.type !== Enums.OfflineMapParametersTypeGenerateGeodatabase)
            return;

        // Get the dictionary of GenerateGeoDatabaseParameters.
        var dictionary = overrides.generateGeodatabaseParameters;

        if (!dictionary.contains(keyForTargetLayer))
            return;

        // Grab the GenerateGeoDatabaseParameters associated with the given key.
        var generateGdbParam = dictionary.value(keyForTargetLayer);

        var table = targetLayer.featureTable;

        // Get the service layer id for the given layer.
        var targetLayerId = table.layerInfo.serviceLayerIdAsInt;

        // Remove the layer option from the list.
        var layerOptions = generateGdbParam.layerOptions;
        var newLayerOptions = [];
        for (var i = 0; i < layerOptions.length; i++) {
            if (layerOptions[i].layerIdAsInt !== targetLayerId) {
                newLayerOptions.push(layerOptions[i]);
            }
        }

        //// Add layer options back to parameters and re-add to the dictionary.
        generateGdbParam.layerOptions =  newLayerOptions;
    }

    function removeServiceConnection() {
        removeFeatureLayer("Service Connection");
    }

    function setHydrantWhereClause(whereClause) {

        if (!overridesReady())
            return;

        var targetLayer = getFeatureLayerByName("Hydrant");
        if (!targetLayer)
            return;

        // Obtain a key for the given basemap-layer.
        var keyForTargetLayer = ArcGISRuntimeEnvironment.createObject("OfflineMapParametersKey", {initLayer: targetLayer});

        if (keyForTargetLayer.empty || keyForTargetLayer.type !== Enums.OfflineMapParametersTypeGenerateGeodatabase)
            return;

        // Get the dictionary of GenerateGeoDatabaseParameters.
        var dictionary = overrides.generateGeodatabaseParameters;

        if (!dictionary.contains(keyForTargetLayer))
            return;

        // Grab the GenerateGeoDatabaseParameters associated with the given key.
        var generateGdbParam = dictionary.value(keyForTargetLayer);

        var table = targetLayer.featureTable;

        // Get the service layer id for the given layer.
        var targetLayerId = table.layerInfo.serviceLayerIdAsInt;

        // Update the where-clause on the layer.
        var layerOptions = generateGdbParam.layerOptions;
        for (var i = 0; i < layerOptions.length; i++) {
            var layerOption = layerOptions[i];
            if (layerOption.layerIdAsInt === targetLayerId) {
                layerOption.whereClause = whereClause;
                layerOption.queryOption = Enums.GenerateLayerQueryOptionUseFilter;
                break;
            }
        }
    }

    function setClipWaterPipesAOI(clip) {

        if (!overridesReady())
            return;

        var targetLayer = getFeatureLayerByName("Main");
        if (!targetLayer)
            return;

        // Obtain a key for the given basemap-layer.
        var keyForTargetLayer = ArcGISRuntimeEnvironment.createObject("OfflineMapParametersKey", {initLayer: targetLayer});

        if (keyForTargetLayer.empty || keyForTargetLayer.type !== Enums.OfflineMapParametersTypeGenerateGeodatabase)
            return;

        // Get the dictionary of GenerateGeoDatabaseParameters.
        var dictionary = overrides.generateGeodatabaseParameters;

        if (!dictionary.contains(keyForTargetLayer))
            return;

        // Grab the GenerateGeoDatabaseParameters associated with the given key.
        var generateGdbParam = dictionary.value(keyForTargetLayer);

        var table = targetLayer.featureTable;

        // Get the service layer id for the given layer.
        var targetLayerId = table.layerInfo.serviceLayerIdAsInt;

        // Set the use geometry flag on the layer.
        var layerOptions = generateGdbParam.layerOptions;
        for (var i = 0; i < layerOptions.length; i++) {
            var layerOption = layerOptions[i];
            if (layerOption.layerIdAsInt === targetLayerId) {
                layerOption.useGeometry = clip;
                break;
            }
        }
    }

    function takeMapOffline() {
        // create the job
        generateJob = offlineMapTask.generateOfflineMapWithOverrides(parameters, outputMapPackage, overrides);

        // check if job is valid
        if (generateJob) {
            // show the export window
            generateWindow.visible = true;

            // connect to the job's status changed signal to know once it is done
            generateJob.jobStatusChanged.connect(updateJobStatus);
            // connect to the job's progress changed signal
            generateJob.progressChanged.connect(updateProgress);

            generateJob.start();
        } else {
            generateWindow.visible = true;
            generateWindow.statusText = "Task failed";
            generateWindow.hideWindow(5000);
        }
    }

    function updateJobStatus() {
        switch(generateJob.jobStatus) {
        case Enums.JobStatusFailed:
            generateWindow.statusText = "Task failed";
            generateWindow.hideWindow(5000);
            break;
        case Enums.JobStatusNotStarted:
            generateWindow.statusText = "Job not started";
            break;
        case Enums.JobStatusPaused:
            generateWindow.statusText = "Job paused";
            break;
        case Enums.JobStatusStarted:
            generateWindow.statusText = "In progress";
            break;
        case Enums.JobStatusSucceeded:
            // show any layer errors
            if (generateJob.result.hasErrors) {
                var layerErrors = generateJob.result.layerErrors;
                var errorText = "";
                for (var i = 0; i < layerErrors.length; i++) {
                    var errorPair = layerErrors[i];
                    errorText += errorPair.layer.name + ": " + errorPair.error.message + "\n";
                }
                msgDialog.detailedText = errorText;
                msgDialog.open();
            }

            // show the map
            generateWindow.statusText = "Complete";
            generateWindow.hideWindow(1500);
            displayOfflineMap(generateJob.result);
            break;
        default:
            console.log("default");
            break;
        }
    }

    function updateProgress() {
        generateWindow.progressText = generateJob.progress;
    }

    function displayOfflineMap(result) {
        // Set the offline map to the MapView
        mapView.map = result.offlineMap;
        downloadButton.visible = false;
        extentRectangle.visible = false;
    }

    OverridesWindow {
        id: overridesWindow
        anchors.fill: parent
        visible: offlineMapTask.createGenerateOfflineMapParameterOverridesStatus === Enums.TaskStatusCompleted

        onBasemapLODSelected: setBasemapLOD(min, max);
        onBasemapBufferChanged: setBasemapBuffer(buffer);
        onRemoveSystemValvesChanged: removeSystemValves();
        onRemoveServiceConnectionChanged: removeServiceConnection();
        onHydrantWhereClauseChanged: setHydrantWhereClause(whereClause);
        onClipWaterPipesAOIChanged: setClipWaterPipesAOI(clip);

        onOverridesAccepted:  takeMapOffline();
    }

    GenerateWindow {
        id: generateWindow
        anchors.fill: parent
    }

    Dialog {
        id: msgDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Layer Errors"
        property alias text : textLabel.text
        property alias detailedText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: "Some layers could not be taken offline."
            }
            Text {
                id: detailsLabel
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: offlineMapTask.createGenerateOfflineMapParameterOverridesStatus === Enums.TaskStatusInProgress ||
                 offlineMapTask.createDefaultGenerateOfflineMapParameterStatus === Enums.TaskStatusInProgress
    }

    /* Uncomment this section when running as standalone application
    AuthenticationView {
        anchors.fill: parent
        authenticationManager: AuthenticationManager
    }
    */
}
