// [WriteFile Name=FilterByDefinitionExpressionOrDisplayFilter, Category=Features]
// [Legal]
// Copyright 2016 Esri.

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

import QtQuick
import QtQuick.Controls
//! [Import the namespace that has QML type that was registered in Cpp]
import Esri.Samples

FilterByDefinitionExpressionOrDisplayFilterSample {
    id: definitionExpressionOrDisplayFilterSample
    width: 800
    height: 600

    //! [Declare map view in QML that will be accessed from Cpp]
    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }
    //! [Declare map view in QML that will be accessed from Cpp]

    Column
    {
        id: expressionColumn
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 5
            bottomMargin: 25
        }
        spacing: 5

        Label {
            text: "Current feature count: " + definitionExpressionOrDisplayFilterSample.currentFeatureCount
        }

        // button to apply a definition expression
        Button {
            text: "Apply Expression"
            enabled: definitionExpressionOrDisplayFilterSample.layerInitialized
            width: 200
            onClicked: {
                // Call C++ invokable function to set the definition expression
                definitionExpressionOrDisplayFilterSample.setDefExpression("req_type = \'Tree Maintenance or Damage\'");
            }
        }

        // button to apply a display filter
        Button {
            text: "Apply Display Filter"
            enabled: definitionExpressionOrDisplayFilterSample.layerInitialized
            width: 200
            onClicked: {
                // Call C++ invokable function to set the display filter
                definitionExpressionOrDisplayFilterSample.setDisplayFilter("req_type = \'Tree Maintenance or Damage\'");
            }
        }

        // button to reset the definition expression
        Button {
            text: "Reset"
            enabled: definitionExpressionOrDisplayFilterSample.layerInitialized
            width: 200
            onClicked: {
                // Call C++ invokable function to reset the definition expression and display filter
                definitionExpressionOrDisplayFilterSample.resetDisplayFilterParams();
                definitionExpressionOrDisplayFilterSample.resetDefExpressionParams();
            }
        }
    }
}
