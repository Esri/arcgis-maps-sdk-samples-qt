// [WriteFile Name=CreateAndSaveMap, Category=Maps]
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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.3

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    StackView {
        id: stackView
        anchors {
            fill: parent
        }

        initialItem: LayerWindow {
            onCreateMapSelected: {
                mapView.visible = true;
                stackView.push(mapView)
                var map = createMap(basemap, layerList);
                mapView.map = map;
            }
        }

        MapView {
            id: mapView
            visible: false

            Button {
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: 5 * scaleFactor
                }
                text: "Save map"

                onClicked: {
                    if (portal.loadStatus !== Enums.LoadStatusLoaded)
                        portal.load();
                    else {
                        options.visible = true;
                        stackView.push(options);
                    }
                }
            }

            Connections {
                target: mapView.map
                onSaveStatusChanged: {
                    if (mapView.map.saveStatus === Enums.TaskStatusCompleted) {
                        console.log("sve complete", mapView.map.item.itemId);
                    } else if (mapView.map.saveStatus === Enums.TaskStatusInProgress) {
                        console.log("in progress");
                    } else if (mapView.map.saveStatus === Enums.TaskStatusErrored) {
                        console.log("error", mapView.map.error.message);
                    }
                }
            }
        }
    }

    SaveOptionsWindow {
        id: options
        visible: false

        onCancelClicked: {
            mapView.visible = true;
            stackView.pop(mapView)
        }

        onSaveMapClicked: {
            var thumbnail = null;
            var folder = null;
            var forceSave = true;
            mapView.map.saveAs(portal, title, tags, forceSave, folder, description, thumbnail);
        }
    }

    // Create Portal object that requires sign in
    Portal {
        id: portal
        url: "https://www.arcgis.com"
        loginRequired: true

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusLoaded) {
                options.visible = true;
                stackView.push(options);
            }
        }
    }

    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
        anchors.fill: parent
        authenticationManager: AuthenticationManager
    }

    function createMap(basemap, layerList) {
        // Create the Basemap
        var selectedBasemap;
        if (basemap === "Streets")
            selectedBasemap = ArcGISRuntimeEnvironment.createObject("BasemapStreets");
        else if (basemap === "Imagery")
            selectedBasemap = ArcGISRuntimeEnvironment.createObject("BasemapImagery");
        else if (basemap === "Topographic")
            selectedBasemap = ArcGISRuntimeEnvironment.createObject("BasemapTopographic");
        else if (basemap === "Oceans")
            selectedBasemap = ArcGISRuntimeEnvironment.createObject("BasemapOceans");

        // Create the Map with the basemap
        var map = ArcGISRuntimeEnvironment.createObject("Map", { basemap: selectedBasemap });

        // Add Operational Layers
        for (var i = 0; i < layerList.length; i++) {
            if (layerList[i] === "WorldElevations") {
                var elevationLyr = ArcGISRuntimeEnvironment.createObject("ArcGISMapImageLayer", {
                                                                             url : "https://sampleserver5.arcgisonline.com/arcgis/rest/services/Elevation/WorldElevations/MapServer"
                                                                         });
                elevationLyr.opacity = 0.5;
                map.operationalLayers.append(elevationLyr);
            } else if (layerList[i] === "Census") {
                map.operationalLayers.append(ArcGISRuntimeEnvironment.createObject("ArcGISMapImageLayer", {
                                                                                       url : "https://sampleserver5.arcgisonline.com/arcgis/rest/services/Census/MapServer"
                                                                                   }));
            }
        }

        // Return the Map
        return map;
    }
}
