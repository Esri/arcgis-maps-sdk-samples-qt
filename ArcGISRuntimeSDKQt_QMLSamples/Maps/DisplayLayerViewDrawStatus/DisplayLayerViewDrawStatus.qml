// [WriteFile Name=DisplayLayerViewDrawStatus, Category=Maps]
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent

        // add a map to the mapView
        Map {
            id: map

            // create tiled layer using url
            ArcGISTiledLayer {
                url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer"
            }

            // create a map image layer using a url
            ArcGISMapImageLayer {
                url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"
                minScale: 40000000
                maxScale: 2000000
            }

            //create a feature layer using a url
            FeatureLayer {
                ServiceFeatureTable {
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"
                }
            }

            // create initial viewpoint
            ViewpointCenter {
                targetScale: 5e7

                Point {
                    x: -11e6
                    y: 45e5
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    for (var i = 0; i < map.operationalLayers.count; i++)
                        layerViewModel.append({"name": map.operationalLayers.get(i).name, "status": "Unknown"});
            }
        }

        onLayerViewStateChanged: {
            // find index of changed layer
            var index = getindex(layer);
            // get Current Status
            var status = viewStatusString(layerViewState);
            // change name if layer loaded
            layerViewModel.setProperty(index, "name", layer.name);
            // update Status in ListModel
            layerViewModel.setProperty(index, "status", status);
        }

        function viewStatusString(layerViewState) {
            var stateFlag = layerViewState.statusFlags;
            if (stateFlag & Enums.LayerViewStatusActive)
                return "Active";
            if (stateFlag & Enums.LayerViewStatusNotVisible)
                return "Not Visible";
            if (stateFlag & Enums.LayerViewStatusOutOfScale)
                return "Out of scale";
            if (stateFlag & Enums.LayerViewStatusLoading)
                return "Loading";
            if (stateFlag & Enums.LayerViewStatusError)
                return "Error";

            return "Unknown";
        }

        function getindex(layer) {
            for (var i = 0; i < layerViewModel.count; i++) {
                if (layer === map.operationalLayers.get(i))
                    return i;
            }
        }
    }

    // table to display layer names and statuses
    TableView {
        id: tableView
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 25 * scaleFactor
        }
        height: 150 * scaleFactor
        width: 230 * scaleFactor
        model: layerViewModel
        headerVisible: false
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        opacity: 0.95

        // set number of layers states to be displayed at once
        rowDelegate: Row {
            height: tableView.height / 3
        }

        // create rectangle to frame the TableView
        style: TableViewStyle {
            backgroundColor: "transparent"
            frame: Rectangle {
                border.color: "black"
                radius: 10

                // make sure mouse actions on table do not affect map behind it
                MouseArea {
                    anchors.fill: parent
                    onClicked: mouse.accepted = true
                    onWheel: wheel.accepted = true
                }
            }
        }

        // create List Model to store Layer View States and names
        ListModel {
            id: layerViewModel
        }

        TableViewColumn {
            role: "name"
            width: tableView.width * 0.75 - tableView.anchors.margins
            delegate: Component {
                Text {
                    text: styleData.value
                    leftPadding: tableView.anchors.margins
                    renderType: Text.NativeRendering
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font {
                        weight: Font.Black
                        pixelSize: tableView.height * 0.10
                    }
                }
            }
        }

        TableViewColumn {
            role: "status"
            width: tableView.width * 0.25
            delegate: Component {
                Text {
                    text: styleData.value
                    renderType: Text.NativeRendering
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: tableView.height * 0.10
                    color: "steelblue"
                }
            }
        }
    }
}
