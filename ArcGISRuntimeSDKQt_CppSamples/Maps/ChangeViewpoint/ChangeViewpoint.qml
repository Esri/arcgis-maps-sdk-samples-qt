// [WriteFile Name=ChangeViewpoint, Category=Maps]
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
import Esri.Samples

ChangeViewpointSample {
    id: changeViewpointSample
    width: 800
    height: 600

    // add a mapView component
    MapView {
        id: mapQuickView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }
    mapView: mapQuickView

    ComboBox {
        id: comboBoxViewpoint
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }

        // Add background to the ComboBox
        Rectangle {
            anchors.fill: parent
            radius: 10
        }

        property int bestWidth: implicitWidth

        width: bestWidth + rightPadding + leftPadding + 20

        model: [ "Center",
                 "Center and scale",
                 "Geometry",
                 "Geometry and padding",
                 "Rotation",
                 "Scale 1:5,000,000",
                 "Scale 1:10,000,000",
                 "Animation" ]

        onCurrentTextChanged: {
            // Call C++ invokable function to change the viewpoint
            changeViewpointSample.changeViewpoint(comboBoxViewpoint.currentText);
        }

        onModelChanged: {
            let w = bestWidth;
            for (let i = 0; i < comboBoxViewpoint.model.length; ++i) {
                metrics.text = comboBoxViewpoint.model[i];
                w = Math.max(w, metrics.width);
            }
            bestWidth = w;
        }

        TextMetrics {
            id: metrics
            font: comboBoxViewpoint.font
        }
    }
}
