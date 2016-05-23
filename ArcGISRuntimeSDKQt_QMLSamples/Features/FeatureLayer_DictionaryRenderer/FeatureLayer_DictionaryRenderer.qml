// [WriteFile Name=FeatureLayer_DictionaryRenderer, Category=Features]
// [Legal]
// Copyright 2015-2016 Esri.

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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    // Create MapView that contains a Map with the Imagery with Labels Basemap
    MapView {
        id: mapView
        anchors.fill: parent
        Map {
            id: map
            BasemapTopographic {}
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }

    ProgressBar {
        id: progressBar_loading
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        indeterminate: true
        visible: true
    }

    SymbolDictionary {
        id: symbolDictionary
        specificationType: "mil2525d"
        dictionaryPath: dataPath + "/styles/mil2525d.stylx"
    }

    Geodatabase {
        id: geodatabase_militaryOverlay
        path: dataPath + "/geodatabase/militaryoverlay.geodatabase"
        autoLoad: true

        Connections {
            property var gdbLayers: []
            target: geodatabase_militaryOverlay
            onLoadStatusChanged: {
                if (Enums.LoadStatusLoaded === geodatabase_militaryOverlay.loadStatus) {
                    var tables = geodatabase_militaryOverlay.geodatabaseFeatureTables;

                    // Create a layer for each table
                    for (var i = tables.length - 1; i >= 0; i--) {
                        var layer = Qt.createQmlObject("import Esri.ArcGISRuntime 100.0; FeatureLayer {}", map);
                        gdbLayers.push(layer);
                        // Each layer needs its own renderer, though all renderers can share the SymbolDictionary.
                        var renderer = Qt.createQmlObject("import Esri.ArcGISRuntime 100.0; DictionaryRenderer {}", layer);
                        renderer.symbolDictionary = symbolDictionary;
                        layer.renderer = renderer;
                        layer.featureTable = tables[i];

                        // Connect the layer's loadStatusChanged signal
                        layer.loadStatusChanged.connect(function () {

                            // See if all the layers have loaded.
                            for (var j = 0; j < gdbLayers.length; j++) {
                                if (Enums.LoadStatusLoaded !== gdbLayers[j].loadStatus) {
                                    return;
                                }
                            }

                            /**
                             * If we get here, all the layers loaded. Union the extents and set
                             * the viewpoint.
                             */
                            var bbox = gdbLayers[0].fullExtent;
                            for (j = 1; j < gdbLayers.length; j++) {
                                bbox = GeometryEngine.unionOf(bbox, gdbLayers[j].fullExtent);
                            }
                            mapView.setViewpointGeometry(bbox);
                            progressBar_loading.visible = false;
                        });

                        // Add the layer to the map
                        map.operationalLayers.append(layer);
                    }
                }
            }
        }
    }

}
