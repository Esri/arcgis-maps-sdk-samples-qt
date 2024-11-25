// [WriteFile Name=DistanceMeasurementAnalysis, Category=Analysis]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {

    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DistanceMeasurementAnalysisSample {
        id: model
        sceneView: view
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
                text: model.directDistance
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
                text: model.verticalDistance
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
                text: model.horizontalDistance
            }
        }
        Row {
            id: row
            spacing: 5
            Text {
                text: "Unit System:"
                color: "white"
            }
            ComboBox {
                id: comboBox
                model: ["Metric", "Imperial"]
                onCurrentTextChanged: model.setUnits(currentText);
            }
        }
    }
}
