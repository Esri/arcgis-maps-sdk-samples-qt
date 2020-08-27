// [WriteFile Name=RasterStretchRenderer, Category=Layers]
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
import Esri.Samples 1.0

RasterStretchRendererSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string minMax: "Min Max"
    readonly property string percentClip: "Percent Clip"
    readonly property string stdDeviation: "Standard Deviation"
    readonly property var stretchTypes: [minMax, percentClip, stdDeviation]
    property bool editingRenderer: false

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
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

        Column {
            anchors {
                top: parent.top
                bottom: parent.bottom
                margins: 24
            }
            width: parent.width
            spacing: 16

            ComboBox {
                id: stretchTypeCombo
                anchors.horizontalCenter: parent.horizontalCenter
                model: stretchTypes
                property int modelWidth: 0
                width: modelWidth + leftPadding + rightPadding + indicator.width
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }
                TextMetrics {
                    id: metrics
                    font: stretchTypeCombo.font
                }
            }

            InputWithLabel {
                id: minMaxMin
                visible: stretchTypeCombo.currentText === minMax
                spacing: 8
                label: "min value"
                maxRange: 255
                value: 0
            }

            InputWithLabel {
                id: minMaxMax
                visible: stretchTypeCombo.currentText === minMax
                spacing: 8
                label: "max value"
                maxRange: 255
                value: 255
            }

            InputWithLabel {
                id: percentClipMin
                visible: stretchTypeCombo.currentText === percentClip
                spacing: 8
                label: "min value"
                maxRange: 100
                value: 0
            }

            InputWithLabel {
                id: percentClipMax
                visible: stretchTypeCombo.currentText === percentClip
                spacing: 8
                label: "max value"
                maxRange: 100
                value: 100
            }

            InputWithLabel {
                id: sdFactor
                visible: stretchTypeCombo.currentText === stdDeviation
                spacing: 8
                label: "factor"
                maxRange: 25
                value: 0
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
        if (stretchTypeCombo.currentText === minMax){
            applyMinMax(minMaxMin.value, minMaxMax.value);
        }
        else if (stretchTypeCombo.currentText === percentClip){
            applyPercentClip(percentClipMin.value, 100 - percentClipMax.value);
        }
        else if (stretchTypeCombo.currentText === stdDeviation){
            applyStandardDeviation(sdFactor.value);
        }
    }
}
