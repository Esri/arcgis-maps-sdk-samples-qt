// [WriteFile Name=BlendRasterLayer, Category=Layers]
// [Legal]
// Copyright 2017 Esri.

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

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    property bool editingRenderer: false
    property bool useColorRamp: colorRampCtrl.value().length === 0

    SlopeTypeModel {
        id: slopeTypeModel
    }

    ColorRampModel {
        id: colorRampModel
    }

    MapView {
        anchors.fill: parent
        Map {
            id: map
            basemap: useColorRamp ?
                         basemap :
                         basemapColorRamp
        }
    }

    Basemap {
        id: basemap
        RasterLayer {
            id: rasterLayer
            Raster {
                id: baseRaster
                path: dataPath + "/Shasta.tif"
            }
        }
    }

    Basemap {
        id: basemapColorRamp
        RasterLayer {
            id: rasterLayerColorRamp

            Raster {
                path: dataPath + "/Shasta_Elevation.tif"
            }
        }
    }

    Raster {
        id: elevationRaster
        path: dataPath + "/Shasta_Elevation.tif"
    }

    Rectangle {
        visible: editButton.visible
        anchors.centerIn: editButton
        radius: 8 * scaleFactor
        height: editButton.height + (16 * scaleFactor)
        width: editButton.width + (16 * scaleFactor)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2 * scaleFactor
        opacity: 0.75
    }

    Button {
        id: editButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 32 * scaleFactor
        }
        visible: rendererBox.width === 0
        text: "Edit Renderer"
        onClicked: editingRenderer = true;
    }

    Rectangle {
        id: rendererBox
        clip: true
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        color: "white"
        opacity: 0.75
        width: editingRenderer ? parent.width : 0

        Column {
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 24 * scaleFactor
            }
            width: parent.width
            spacing: 16 * scaleFactor

            TextWithSlider {
                id: altitudeCtrl
                label: "altitde"
                min: 0
                max: 90
            }

            TextWithSlider {
                id: azimithCtrl
                label: "azimuth"
                min: 0
                max: 359
            }

            TextWithComboBox {
                id: slopeTypeCtrl
                label: "slope type"
                model: slopeTypeModel
            }

            TextWithComboBox {
                id: colorRampCtrl
                label: "color ramp"
                model: colorRampModel
            }

            Button {
                text: "Render"
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    editingRenderer = false;
                    applyRendererSettings();
                }
            }
        }

        Behavior on width { PropertyAnimation { duration: 500 } }
    }

    function applyRendererSettings() {
        var blendRenderer = ArcGISRuntimeEnvironment.createObject("BlendRenderer");
        blendRenderer.elevationRaster = elevationRaster;
        blendRenderer.altitude = altitudeCtrl.sliderValue;
        blendRenderer.azimuth = azimithCtrl.sliderValue;
        blendRenderer.slopeType = slopeTypeCtrl.value();
        blendRenderer.colorRamp = getColorRamp();
        blendRenderer.outputBitDepth = 8;
        blendRenderer.gammas = [];
        blendRenderer.noDataValues = [];
        blendRenderer.outputMaxValues = [255];
        blendRenderer.outputMinValues = [9];
        blendRenderer.sourceMaxValues = [];
        blendRenderer.sourceMinValues = [];

        applyRenderer(blendRenderer);
    }

    function applyRenderer(blendRenderer) {
        if (colorRampCtrl.value().length === 0)
            rasterLayer.renderer = blendRenderer;
        else
            rasterLayerColorRamp.renderer = blendRenderer;
    }

    function getColorRamp() {
        if (colorRampCtrl.value().length === 0)
            return null;

        var colorRamp = ArcGISRuntimeEnvironment.createObject(colorRampCtrl.value());
        colorRamp.size = 800;

        return colorRamp;
    }
}
