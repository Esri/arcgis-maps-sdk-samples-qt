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
        color: palette.base
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
            Label {
                text: qsTr("Direct Distance:")
            }
            Label {
                id: directDistanceText
                text: model.directDistance
            }
        }
        Row {
            spacing: 5
            Label {
                text: qsTr("Vertical Distance:")
            }
            Label {
                id: verticalDistanceText
                text: model.verticalDistance
            }
        }
        Row {
            spacing: 5
            Label {
                text: qsTr("Horizontal Distance:")
            }
            Label {
                id: horizontalDistanceText
                text: model.horizontalDistance
            }
        }
        Row {
            id: row
            spacing: 5
            Label {
                text: qsTr("Unit System:")
            }
            ComboBox {
                id: comboBox
                model: ["Metric", "Imperial"]
                onCurrentTextChanged: model.setUnits(currentText);
            }
        }
    }
}
