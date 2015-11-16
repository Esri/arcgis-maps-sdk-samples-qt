// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property int mapImageSublayerIndex: 0
    property int tiledSublayerIndex: 0

    // Create the MapView
    MapView {
        anchors.fill: parent
        // Nest the Map as a child of the MapView
        Map {
            id: map
            // Nest the Basemap to add it as the Map's Basemap
            BasemapTopographic {}

            // Add a tiled layer as an operational layer
            ArcGISTiledLayer {
                id: tiledLayer
                url: "http://services.arcgisonline.com/ArcGIS/rest/services/Specialty/Soil_Survey_Map/MapServer"

                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded) {
                        getTiledLayerInfos();
                    }
                }
            }

            // Add a map image layer as an operational layer
            ArcGISMapImageLayer {
                id: mapImageLayer
                url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"

                // On Loaded request the legend info
                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded) {
                        getMapImageLayerInfos();
                    }
                }
            }

            // Add a feature layer as an operational layer
            FeatureLayer {
                id: featureLayer
                featureTable: ServiceFeatureTable {
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"
                }

                // On Loaded request the legend info
                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded) {
                        featureLayer.fetchLegendInfos();
                    }
                }

                // Iterate through the legend infos and write name and URL to the model
                onFetchLegendInfosStatusChanged: {
                    if (fetchLegendInfosStatus === Enums.TaskStatusCompleted) {
                        var legendResults = legendInfosResult;
                        for (var i = 0; i < legendResults.length; i++) {
                            var legendResult = legendResults[i];
                            var legendDict = {itemName: legendResult.name, itemUrl: Qt.resolvedUrl(legendResult.symbol.url)};
                            recreationListModel.append(legendDict);
                        }
                    }
                }
            }

            // Set the initial viewpoint
            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -11e6
                    y: 6e6
                    spatialReference: SpatialReference {wkid: 102100}
                }
                scale: 9e7
            }
        }
    }

    // Create outter rectangle for the legend
    Rectangle {
        id: legendRect
        anchors {
            margins: 10 * scaleFactor
            left: parent.left
            top: parent.top
        }
        property bool expanded: false
        height: 45 * scaleFactor
        width: 200 * scaleFactor
        color: "lightgrey"
        opacity: 0.95
        radius: 10
        clip: true
        border {
            color: "darkgrey"
            width: 1
        }

        // Animate the expand and collapse of the legend
        Behavior on height {
            SpringAnimation {
                spring: 3
                damping: .4
            }
        }

        // Catch mouse signals so they don't propagate to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        // Create UI for the user to select the layer to display
        Column {
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 5

            Row {
                spacing: 2

                // Combo box to allow switching between layers
                ComboBox {
                    model: ["Soil Survey", "Census", "Recreation"]
                    width: 150 * scaleFactor

                    // Switch out the list view's model for each item
                    onCurrentTextChanged: {
                        if (currentText === "Census") {
                            legendListView.model = censusListModel;
                        } else if (currentText === "Recreation") {
                            legendListView.model = recreationListModel;
                        } else if (currentText === "Soil Survey") {
                            legendListView.model = soilListModel;
                        }
                    }
                }

                // Legend icon to allow expanding and collapsing
                Image {
                    source: legendRect.expanded ? "qrc:/Samples/Display Information/ShowLegend/ic_menu_legendpopover_light_d.png" : "qrc:/Samples/Display Information/ShowLegend/ic_menu_legendpopover_light.png"
                    width: 28 * scaleFactor
                    height: width

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (legendRect.expanded) {
                                legendRect.height = 45 * scaleFactor;
                                legendRect.expanded = false;
                            } else {
                                legendRect.height = 200 * scaleFactor;
                                legendRect.expanded = true;
                            }
                        }
                    }
                }
            }

            // Create a list view to display the legend
            ListView {
                id: legendListView
                anchors.margins: 10 * scaleFactor
                width: 180 * scaleFactor
                height: 150 * scaleFactor
                clip: true

                // Create delegate to display the name with an image
                delegate: Item {
                    width: parent.width
                    height: 55 * scaleFactor
                    clip: true

                    Row {
                        spacing: 5
                        anchors.verticalCenter: parent.verticalCenter
                        Image {
                            width: 30 * scaleFactor
                            height: width
                            source: itemUrl
                        }
                        Text {
                            width: 125 * scaleFactor
                            text: itemName
                            wrapMode: Text.WordWrap
                            font.pixelSize: 12 * scaleFactor
                        }

                    }
                }
            }
        }
    }

    // Create a list model for each layer
    ListModel { id: recreationListModel }
    ListModel { id: censusListModel }
    ListModel { id: soilListModel }

    // iterate through the map image sublayers and request legend info for all sublayers
    function getMapImageLayerInfos() {
        if (mapImageSublayerIndex < mapImageLayer.mapImageSublayers.count) {
            var subLayer = mapImageLayer.mapImageSublayers.get(mapImageSublayerIndex);
            subLayer.fetchLegendInfos();
            subLayer.fetchLegendInfosStatusChanged.connect(function () {
                if (subLayer.fetchLegendInfosStatus === Enums.TaskStatusCompleted) {
                    var legendResults = subLayer.legendInfosResult;
                    for (var i = 0; i < legendResults.length; i++) {
                        var legendResult = legendResults[i];
                        // Create a key value pair of names and image URLs for the list model
                        var legendDict = {itemName: subLayer.name + " " + legendResult.name, itemUrl: Qt.resolvedUrl(legendResult.symbol.url)};
                        // Add the JSON to the list model
                        censusListModel.append(legendDict);
                    }
                }
                mapImageSublayerIndex++;
                getMapImageLayerInfos(); // call again recursively after the fetchLegendInfos completes
            });
        }
    }

    // iterate through the tiled sublayers and request legend info for all sublayers
    function getTiledLayerInfos() {
        if (tiledSublayerIndex < tiledLayer.tiledSublayers.length) {
            var subLayer = tiledLayer.tiledSublayers[tiledSublayerIndex];
            subLayer.fetchLegendInfos();
            subLayer.fetchLegendInfosStatusChanged.connect(function () {
                if (subLayer.fetchLegendInfosStatus === Enums.TaskStatusCompleted) {
                    var legendResults = subLayer.legendInfosResult;
                    for (var i = 0; i < legendResults.length; i++) {
                        var legendResult = legendResults[i];
                        // Create a key value pair of names and image URLs for the list model
                        var legendDict = {itemName: subLayer.name + " " + legendResult.name, itemUrl: Qt.resolvedUrl(legendResult.symbol.url)};
                        // Add the JSON to the list model
                        soilListModel.append(legendDict);
                    }
                }
                tiledSublayerIndex++;
                getTiledLayerInfos(); // call again recursively after the fetchLegendInfos completes
            });
        }
    }

    // neatline
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
