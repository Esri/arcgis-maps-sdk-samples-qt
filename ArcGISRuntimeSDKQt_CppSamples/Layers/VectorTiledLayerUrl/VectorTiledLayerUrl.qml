// [WriteFile Name=VectorTiledLayerUrl, Category=Layers]
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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.Samples 1.0

VectorTiledLayerUrlSample {
    id: vectorTiledLayerUrlSample
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
    }

    ComboBox {
        id: comboBoxBasemap
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        property int modelWidth: 0
        width: modelWidth + leftPadding + rightPadding + indicator.width
        model: ["Mid-Century","Colored Pencil","Newspaper","Nova","World Street Map (Night)"]
        onCurrentTextChanged: {
            // Call C++ invokable function to switch the basemaps
            vectorTiledLayerUrlSample.changeBasemap(currentText);
        }

        Component.onCompleted : {
            for (let i = 0; i < model.length; ++i) {
                metrics.text = model[i];
                modelWidth = Math.max(modelWidth, metrics.width);
            }
        }
        TextMetrics {
            id: metrics
            font: comboBoxBasemap.font
        }
    }
}
