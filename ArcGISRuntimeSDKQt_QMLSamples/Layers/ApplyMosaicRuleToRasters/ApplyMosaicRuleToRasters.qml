// [WriteFile Name=ApplyMosaicRuleToRasters, Category=Layers]
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
import Esri.ArcGISRuntime 100.10

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var rasterRules: ["None","NorthWest","Center","ByAttribute","LockRaster"]
    property var mosaicRule: null

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographicVector {}

            RasterLayer {
                id: rasterLayer
                ImageServiceRaster {
                    id: imageServiceRaster
                    url: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/amberg_germany/ImageServer"
                }
                onLoadStatusChanged: {
                    // Once loaded set the viewpoint to the center of the raster layer
                    if (loadStatus === Enums.LoadStatusLoaded) {
                        mapView.setViewpointCenterAndScale(rasterLayer.fullExtent.center, 25000);

                        // Create a mosaic rule and set the mosaic rule property of the image service raster
                        mosaicRule = ArcGISRuntimeEnvironment.createObject("MosaicRule");
                        imageServiceRaster.mosaicRule = mosaicRule;
                    }
                }
            }
        }

        Rectangle {
            width: childrenRect.width
            height: childrenRect.height
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: mapView.horizontalCenter
            }
            color: "lightgrey"
            opacity: 0.8
            visible: rasterLayer.loadStatus === Enums.LoadStatusLoaded ? true : false

            Column {
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Select Mosaic Rule")
                }
                ComboBox {
                    id: rasterRulesComboBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: rasterRules

                    onActivated: applyRasterRule(rasterRules[currentIndex]);
                }
            }
        }
    }

    function applyRasterRule(ruleString) {
        // Reset to clear previous mosaic rule parameters
        resetMosaicRule();

        if (ruleString === "None") {
            mosaicRule.mosaicMethod = Enums.MosaicMethodNone;

        } else if (ruleString === "NorthWest") {
            mosaicRule.mosaicMethod = Enums.MosaicMethodNorthwest;
            mosaicRule.mosaicOperation = Enums.MosaicOperationFirst;

        } else if (ruleString === "Center") {
            mosaicRule.mosaicMethod = Enums.MosaicMethodCenter;
            mosaicRule.mosaicOperation = Enums.MosaicOperationBlend;

        } else if (ruleString === "ByAttribute") {
            mosaicRule.mosaicMethod = Enums.MosaicMethodAttribute;
            mosaicRule.sortField = "OBJECTID";

        } else if (ruleString === "LockRaster") {
            mosaicRule.mosaicMethod = Enums.MosaicMethodLockRaster;
            mosaicRule.lockRasterIds = ["1","7","12"];
        }
        imageServiceRaster.mosaicRule = mosaicRule;
    }

    // Helper function to reset the mosaic rule
    function resetMosaicRule() {
        mosaicRule = null;
        mosaicRule = ArcGISRuntimeEnvironment.createObject("MosaicRule");
    }
}
