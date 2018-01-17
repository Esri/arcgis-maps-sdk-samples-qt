// [WriteFile Name=RasterRgbRenderer, Category=Layers]
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
    property string minMax: "Min Max"
    property string percentClip: "Percent Clip"
    property string stdDeviation: "Standard Deviation"
    property var stretchTypes: [minMax, percentClip, stdDeviation]
    property bool editingRenderer: false

    MapView {
        anchors.fill: parent

        Map {
            Basemap {
                RasterLayer {
                    id: rasterLayer

                    Raster {
                        path: dataPath + "/Shasta.tif"
                    }
                }
            }
        }
    }

    MinMaxStretchParameters {
        id: minMaxParams
    }

    PercentClipStretchParameters {
        id: percentClipParams
    }

    StandardDeviationStretchParameters {
        id: stdDevParams
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
        visible: width > 0

        Column {
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 24 * scaleFactor
            }
            width: parent.width
            spacing: 16 * scaleFactor

            ComboBox {
                id: stretchTypeCombo
                anchors.horizontalCenter: parent.horizontalCenter
                width: 175 * scaleFactor
                model: stretchTypes
            }

            MinMaxRow {
                id: minMaxMin
                visible: stretchTypeCombo.currentText === minMax
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8 * scaleFactor
                isMin: true
                maxRange: 255
                numVals: 3
            }

            MinMaxRow {
                id: minMaxMax
                visible: stretchTypeCombo.currentText === minMax
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8 * scaleFactor
                isMin: false
                maxRange: 255
                numVals: 3
            }

            MinMaxRow {
                id: percentClipMin
                visible: stretchTypeCombo.currentText === percentClip
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8 * scaleFactor
                isMin: true
                maxRange: 100
                numVals: 1
            }

            MinMaxRow {
                id: percentClipMax
                visible: stretchTypeCombo.currentText === percentClip
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8 * scaleFactor
                isMin: false
                maxRange: 100
                numVals: 1
            }

            Row {
                visible: stretchTypeCombo.currentText === stdDeviation
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8 * scaleFactor
                Text {
                    text: "Factor"
                    anchors.verticalCenter: parent.verticalCenter
                }

                SpinBox {
                    id: sdFactor
                    anchors.verticalCenter: parent.verticalCenter
                    width: 75 * scaleFactor
                    decimals: 2
                    minimumValue: 0
                    maximumValue: 25
                    value: 0
                }
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

    function applyRendererSettings(){
        var rgbRenderer = ArcGISRuntimeEnvironment.createObject("RGBRenderer");

        if (stretchTypeCombo.currentText === minMax){
            minMaxParams.minValues = [minMaxMin.value(0), minMaxMin.value(1), minMaxMin.value(2)];
            minMaxParams.maxValues = [minMaxMax.value(0), minMaxMax.value(1), minMaxMax.value(2)];
            rgbRenderer.stretchParameters = minMaxParams;
        }
        else if (stretchTypeCombo.currentText === percentClip){
            percentClipParams.min =  percentClipMin.value(0);
            percentClipParams.max = 100 - percentClipMax.value(0);
            rgbRenderer.stretchParameters = percentClipParams;
        }
        else if (stretchTypeCombo.currentText === stdDeviation){
            stdDevParams.factor =  sdFactor.value;
            rgbRenderer.stretchParameters = stdDevParams;
        }

        rasterLayer.renderer = rgbRenderer;
    }
}
