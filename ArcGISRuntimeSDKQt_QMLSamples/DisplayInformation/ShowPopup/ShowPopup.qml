// [WriteFile Name=ShowPopup, Category=DisplayInformation]
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
import Esri.ArcGISRuntime.Toolkit.Controls 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var popupManagers: []
    property var featureLayer: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            initUrl: "https://runtime.maps.arcgis.com/home/webmap/viewer.html?webmap=e4c6eb667e6c43b896691f10cc2f1580"
        }

        // identify layers on mouse click
        onMouseClicked: {
            const screenX = mouse.x;
            const screenY = mouse.y;
            const tolerance = 12;
            const returnPopupsOnly = false;
            featureLayer = mapView.map.operationalLayers.get(0);
            mapView.identifyLayer(featureLayer, screenX, screenY, tolerance, returnPopupsOnly);
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus !== Enums.TaskStatusCompleted)
                return;

            // if layer is a feature layer, select the identify result features
            if (featureLayer.layerType === Enums.LayerTypeFeatureLayer) {
                featureLayer.clearSelection();
                const geoElements = mapView.identifyLayerResult.geoElements;

                for (let i = 0; i < geoElements.length; i++) {
                    featureLayer.selectFeature(geoElements[i]);
                }
            }

            const popups = mapView.identifyLayerResult.popups;
            // clear the list of PopupManagers
            popupManagers = [];

            for (let i = 0; i < popups.length; i++) {
                // create a popup manager
                const popupManager = ArcGISRuntimeEnvironment.createObject("PopupManager", {
                                                                               popup: popups[i]
                                                                           });
                // push popup manager to list
                popupManagers.push(popupManager);
                popupStackView.popupManagers = popupManagers;
            }
        }
    }

    PopupStackView {
        id: popupStackView
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        onPopupManagersChanged: popupStackView.show();

        onVisibleChanged: {
            if (!visible) {
                if (featureLayer.layerType === Enums.LayerTypeFeatureLayer)
                    featureLayer.clearSelection();
            }
        }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: mapView.identifyLayerStatus === Enums.TaskStatusInProgress
    }
}
