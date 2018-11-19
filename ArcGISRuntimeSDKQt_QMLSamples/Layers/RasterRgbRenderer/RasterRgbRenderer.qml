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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster"
    property string minMax: "Min Max"
    property string percentClip: "Percent Clip"
    property string stdDeviation: "Standard Deviation"
    property var stretchTypes: [minMax, percentClip, stdDeviation]
    property bool editingRenderer: false
    property string selectedType: stretchTypeCombo.currentText

    states: [
        State {
            name: "orientHorizontal"
            when: width > height
            PropertyChanges {
                target: layout
                flow: GridLayout.LeftToRight
                labelAlignment: Qt.AlignRight
                columns: 4
            }
        },
        State {
            name: "orientVertical"
            when: width <= height
            PropertyChanges {
                target: layout
                flow: GridLayout.TopToBottom
                labelAlignment: Qt.AlignLeft
                rows: 4
            }
        }

    ]

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
        visible: width > 0
        ComboBox {
            id: stretchTypeCombo
            anchors {
                bottom: layout.top
                left: layout.left
                right: layout.right
                margins: 5
            }

            model: stretchTypes
            property int modelWidth: 0
            Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
            Component.onCompleted : {
                for (var i = 0; i < model.length; ++i) {
                    metrics.text = model[i];
                    modelWidth = Math.max(modelWidth, metrics.width);
                }
            }
            TextMetrics {
                id: metrics
                font: stretchTypeCombo.font
            }
        }

        GridLayout {
            id: layout
            property int labelAlignment
            anchors {
                centerIn: parent
                margins: 24
            }

            Text {
                text: "Min"
                Layout.alignment: layout.labelAlignment
                visible: selectedType === minMax
            }

            Repeater {
                id: minMaxMin
                model: 3
                SpinBox {
                    editable: true
                    visible: selectedType === minMax
                    from: 0
                    to: 255
                    value: from
                }
            }

            Text {
                text: "Max"
                Layout.alignment: layout.labelAlignment
                visible: selectedType === minMax
            }

            Repeater {
                id: minMaxMax
                model: 3
                SpinBox {
                    editable: true
                    visible: selectedType === minMax
                    from: 0
                    to: 255
                    value: to
                }
            }

            Text {
                text: "Min"
                Layout.alignment: layout.labelAlignment
                visible: selectedType === percentClip
            }

            SpinBox {
                id: percentClipMin
                editable: true
                visible: selectedType === percentClip
                Layout.columnSpan: 3
                from: 0
                to: 100
                value: from
            }

            Text {
                text: "Max"
                Layout.alignment: layout.labelAlignment
                visible: selectedType === percentClip
            }

            SpinBox {
                id: percentClipMax
                editable: true
                visible: selectedType === percentClip
                Layout.columnSpan: 3
                from: 0
                to: 100
                value: to
            }

            Text {
                text: "Factor"
                Layout.alignment: layout.labelAlignment
                visible: selectedType === stdDeviation
            }

            SpinBox {
                id: sdFactor
                editable: true
                visible: selectedType === stdDeviation
                Layout.columnSpan: 3
                property int decimals: 2
                property real realValue: value / 100
                from: 0
                to: 25 * 100
                value: 0

                validator: DoubleValidator {
                    bottom: Math.min(sdFactor.from, sdFactor.to)
                    top: Math.min(sdFactor.from, sdFactor.to)
                }

                textFromValue: function(value, locale) {
                    return Number(value / 100).toLocaleString(locale, 'f', sdFactor.decimals);
                }

                valueFromText: function(text, locale) {
                    return Number.fromLocaleString(locale, text) * 100;
                }
            }
        }

        Button {
            text: "Render"
            anchors {
                top: layout.bottom
                left: layout.left
                right: layout.right
                margins: 5
            }
            onClicked: {
                editingRenderer = false;
                applyRendererSettings();
            }
        }


        Behavior on width { PropertyAnimation { duration: 500 } }
    }

    function applyRendererSettings(){
        var rgbRenderer = ArcGISRuntimeEnvironment.createObject("RGBRenderer");

        if (stretchTypeCombo.currentText === minMax){
            minMaxParams.minValues = [minMaxMin.itemAt(0).value, minMaxMin.itemAt(1).value, minMaxMin.itemAt(2).value];
            minMaxParams.maxValues = [minMaxMax.itemAt(0).value, minMaxMax.itemAt(1).value, minMaxMax.itemAt(2).value];
            rgbRenderer.stretchParameters = minMaxParams;
        }
        else if (stretchTypeCombo.currentText === percentClip){
            percentClipParams.min =  percentClipMin.value;
            percentClipParams.max = 100 - percentClipMax.value;
            rgbRenderer.stretchParameters = percentClipParams;
        }
        else if (stretchTypeCombo.currentText === stdDeviation){
            stdDevParams.factor =  sdFactor.realValue;
            rgbRenderer.stretchParameters = stdDevParams;
        }

        rasterLayer.renderer = rgbRenderer;
    }
}
