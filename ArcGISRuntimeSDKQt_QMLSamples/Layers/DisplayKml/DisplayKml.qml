// [WriteFile Name=DisplayKml, Category=Layers]
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
import QtQuick.Window 2.3
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImageryWithLabels{}

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }
        }
    }

    ViewpointCenter {
        id: viewpoint

        targetScale: 10000000
        center: Point {
            x: -98
            y: 39
            spatialReference: SpatialReference {
                wkid: 4326
            }
        }

        Camera {
            distance: viewpoint.targetScale
            heading: 0
            pitch: 0
            roll: 0
            location: viewpoint.center
        }
    }

    ComboBox {
        id: comboBox
        anchors {
            right: parent.right
            top: parent.top
            margins: 5
        }

        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width

        model: ["URL", "Local file", "Portal Item"]

        onCurrentIndexChanged: {
            // clear previous layers
            scene.operationalLayers.clear();

            // create KML variable
            var kmlLayer;

            // create the Layer
            if (currentIndex === 0)
                kmlLayer = createFromUrl();
            else if (currentIndex === 1)
                kmlLayer = createFromFile();
            else
                kmlLayer = createFromPortalItem();

            // add the layer to the scene
            scene.operationalLayers.append(kmlLayer);

            // zoom to center on the United States
            sceneView.setViewpoint(viewpoint)
        }

        Component.onCompleted : {
            for (var i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width);
            }
            currentIndexChanged();
        }

        TextMetrics {
            id: metrics
            font: comboBox.font
        }
    }

    function createFromUrl() {
        // create the dataset from an online URL
        var kmlDataset = ArcGISRuntimeEnvironment.createObject("KmlDataset", {
                                                                   url: "https://www.wpc.ncep.noaa.gov/kml/noaa_chart/WPC_Day1_SigWx.kml"
                                                               });

        // create the layer
        var kmlLayer = ArcGISRuntimeEnvironment.createObject("KmlLayer", {
                                                                 dataset: kmlDataset
                                                             });

        // return the KML Layer
        return kmlLayer;
    }

    function createFromFile() {
        // create the dataset from a local file
        var kmlDataset = ArcGISRuntimeEnvironment.createObject("KmlDataset", {
                                                                   url: System.userHomePath + "/ArcGIS/Runtime/Data/kml/US_State_Capitals.kml"
                                                               });

        // create the layer
        var kmlLayer = ArcGISRuntimeEnvironment.createObject("KmlLayer", {
                                                                 dataset: kmlDataset
                                                             });

        // return the KML Layer
        return kmlLayer;
    }

    function createFromPortalItem() {
        // create the portal item with an item ID
        var portalItem = ArcGISRuntimeEnvironment.createObject("PortalItem", {
                                                                   itemId: "9fe0b1bfdcd64c83bd77ea0452c76253"
                                                               });

        // create the layer
        var kmlLayer = ArcGISRuntimeEnvironment.createObject("KmlLayer", {
                                                                 item: portalItem
                                                             });

        // return the KML Layer
        return kmlLayer;
    }
}
