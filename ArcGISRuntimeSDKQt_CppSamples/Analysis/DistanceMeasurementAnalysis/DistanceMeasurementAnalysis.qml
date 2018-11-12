// [WriteFile Name=DistanceMeasurementAnalysis, Category=Analysis]
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
import QtQuick.Window 2.2
import Esri.Samples 1.0

DistanceMeasurementAnalysisSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    
    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    Rectangle {
        anchors {
            fill: resultsColumn
            margins: -5
        }
        color: "black"
        opacity: 0.5
        radius: 5
    }

    Column {
        id: resultsColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Row {
            spacing: 5
            Text {
                text: "Direct Distance:"
                color: "white"
            }
            Text {
                id: directDistanceText
                color: "white"
                text: directDistance
            }
        }
        Row {
            spacing: 5
            Text {
                text: "Vertical Distance:"
                color: "white"
            }
            Text {
                id: verticalDistanceText
                color: "white"
                text: verticalDistance
            }
        }
        Row {
            spacing: 5
            Text {
                text: "Horizontal Distance:"
                color: "white"
            }
            Text {
                id: horizontalDistanceText
                color: "white"
                text: horizontalDistance
            }
        }
        Row {
            spacing: 5
            Text {
                text: "Unit System:"
                color: "white"
            }
            ComboBox {
                id: comboBox
                property int modelWidth: 0
                width: modelWidth + leftPadding + rightPadding + indicator.width
                model: ["Metric", "Imperial"]
                onCurrentTextChanged: setUnits(currentText);
                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }

                TextMetrics {
                    id: metrics
                    font: comboBox.font
                }
            }
        }
    }
}
