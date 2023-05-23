// [WriteFile Name=SpatialOperations, Category=Geometry]
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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Esri.Samples

SpatialOperationsSample {
    id: rootRectangle
    clip: true
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

    // Display a ComboBox with options for each operation
    ComboBox {
        id: comboBox
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }

        // Add background to the ComboBox
        Rectangle {
            anchors.fill: parent
            radius: 10
        }

        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding
        model: geometryOperations
        onCurrentIndexChanged: applyGeometryOperation(currentIndex);
        Component.onCompleted : {
            for (let i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width + 20);
            }
        }
        TextMetrics {
            id: metrics
            font: comboBox.font
        }
    }
}
