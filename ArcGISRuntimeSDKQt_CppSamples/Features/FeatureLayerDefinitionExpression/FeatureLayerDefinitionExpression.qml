// [WriteFile Name=FeatureLayerDefinitionExpression, Category=Features]
// [Legal]
// Copyright 2015 Esri.

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

import QtQuick 2.3
import QtQuick.Controls 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

FeatureLayerDefinitionExpressionSample {
    id: definitionExpressionSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Row {
        id: expressionRow
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 5
        }
        spacing: 5

        // button to apply a definition expression
        Button {
            text: "Apply Expression"
            enabled: definitionExpressionSample.layerInitialized
            onClicked: {
                // Call C++ invokable function to set the definition expression
                definitionExpressionSample.setDefExpression("req_Type = \'Tree Maintenance or Damage\'");
            }
        }

        // button to reset the definition expression
        Button {
            text: "Reset"
            enabled: definitionExpressionSample.layerInitialized
            onClicked: {
                // Call C++ invokable function to reset the definition expression
                definitionExpressionSample.setDefExpression("");
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
