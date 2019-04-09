// [WriteFile Name=FeatureLayer_DictionaryRenderer, Category=Features]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600
    
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    // Create MapView that contains a Map with the Topographic Basemap
    MapView {
        id: mapView
        anchors {
            fill: parent
        }
        Map {
            id: map
            BasemapTopographic {}
        }
    }

    ProgressBar {
        id: progressBar_loading
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 5
        }
        indeterminate: true
    }

    //! [Create Dictionary Symbol Style QML]
    DictionarySymbolStyle {
        id: dictionarySymbolStyle
        specificationType: "mil2525d"
        styleLocation: dataPath + "/styles/mil2525d.stylx"
    }
    //! [Create Dictionary Symbol Style QML]

    Geodatabase {
        property var gdbLayers: []

        id: geodatabase_militaryOverlay
        path: dataPath + "/geodatabase/militaryoverlay.geodatabase"

        onLoadStatusChanged: {
            if (Enums.LoadStatusLoaded === geodatabase_militaryOverlay.loadStatus) {
                var tables = geodatabase_militaryOverlay.geodatabaseFeatureTables;

                // Create a layer for each table
                for (var i = tables.length - 1; i >= 0; i--) {
                    //! [Apply Dictionary Renderer Feature Layer QML]
                    // Create a layer and set the feature table
                    var layer = ArcGISRuntimeEnvironment.createObject("FeatureLayer");
                    layer.featureTable = tables[i];

                    // Create a dictionary renderer and apply to the layer
                    var renderer = ArcGISRuntimeEnvironment.createObject(
                                "DictionaryRenderer",
                                { dictionarySymbolStyle: dictionarySymbolStyle });
                    layer.renderer = renderer;
                    //! [Apply Dictionary Renderer Feature Layer QML]

                    /**
                     * If the field names in your data don't match the contents of DictionarySymbolStyle::symbologyFieldNames(),
                     * you must set DictionaryRenderer::symbologyFieldOverrides to a map of key-value pairs like this:
                     * {
                     *   "dictionaryFieldName1": "myFieldName1",
                     *   "dictionaryFieldName2": "myFieldName2"
                     * }
                     * The following commented-out code demonstrates one way to do it, in a scenario where the dictionary
                     * expects the field name "identity" but the database table contains the field "affiliation" instead.
                     */
                    /**
                    var fieldOverrides = {
                        identity: "affiliation"
                    };
                    renderer.symbologyFieldOverrides = fieldOverrides;
                    */

                    gdbLayers.push(layer);

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
