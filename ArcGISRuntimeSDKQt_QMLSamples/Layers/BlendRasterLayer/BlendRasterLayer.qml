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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    property bool editingRenderer: false
    property bool useColorRamp: colorCombo.currentText !== "none"

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
                         basemapColorRamp :
                         basemap

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
        radius: 8
        height: editButton.height + (16)
        width: editButton.width + (16)
        color: "lightgrey"
        border.color: "darkgrey"
        border.width: 2
        opacity: 0.75
    }

    Button {
        id: editButton
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 32
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

        GridLayout {
            anchors {
                centerIn: parent
                margins: 24
            }

            columns: 2

            Text {
                text: "altitude"
            }

            SpinBox {
                id: altSlider
                from: 0
                to: 90
                editable: true
                textFromValue: function(v) {
                    return v.toFixed(0) + "\u00B0";
                }
            }

            Text {
                text: "azimuth"
            }

            SpinBox {
                id: azimuthSlider
                from: 0
                to: 90
                editable: true
                textFromValue: function(v) {
                    return v.toFixed(0) + "\u00B0";
                }
            }


            Text {
                text: "slope type"
            }

            ComboBox {
                id: slopeCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.fillWidth: true
                textRole: "name"
                model: slopeTypeModel

                Component.onCompleted : {
                    for (let i = 0; i < model.count; ++i) {
                        metrics.text = model.get(i).name;
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }
                TextMetrics {
                    id: metrics
                    font: slopeCombo.font
                }
            }


            Text {
                text: "color ramp"
            }

            ComboBox {
                id: colorCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.fillWidth: true
                textRole: "name"
                model: colorRampModel

                Component.onCompleted : {
                    for (let i = 0; i < model.count; ++i) {
                        metrics2.text = model.get(i).name;
                        modelWidth = Math.max(modelWidth, metrics2.width);
                    }
                }
                TextMetrics {
                    id: metrics2
                    font: colorCombo.font
                }
            }

            Button {
                text: "Render"
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    editingRenderer = false;
                    applyRendererSettings();
                }
            }
        }

        Behavior on width { PropertyAnimation { duration: 500 } }
    }

    function applyRendererSettings() {
        const blendRenderer = ArcGISRuntimeEnvironment.createObject("BlendRenderer");
        blendRenderer.elevationRaster = elevationRaster;
        blendRenderer.altitude = altSlider.value;
        blendRenderer.azimuth = azimuthSlider.value;
        blendRenderer.slopeType = slopeTypeModel.get(slopeCombo.currentIndex).value;
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
        const val = colorRampModel.get(colorCombo.currentIndex).value;
        if (val.length === 0)
            rasterLayer.renderer = blendRenderer;
        else
            rasterLayerColorRamp.renderer = blendRenderer;
    }

    function getColorRamp() {
        const val = colorRampModel.get(colorCombo.currentIndex).value;
        if (val.length === 0)
            return null;

        const colorRamp = ArcGISRuntimeEnvironment.createObject(val);
        colorRamp.size = 800;

        return colorRamp;
    }
}
