// [WriteFile Name=ChangeBasemap, Category=Maps]
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

ChangeBasemapSample {
    id: changeBasemapSample
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

        property int bestWidth: implicitWidth

        width: bestWidth + indicator.width + leftPadding + rightPadding

        model: ["Topographic","Streets",
            "Streets (Vector)",
            "Streets - Night (Vector)",
            "Imagery (Raster)",
            "Imagery with Labels (Raster)",
            "Imagery with Labels (Vector)",
            "Dark Gray Canvas (Vector)",
            "Light Gray Canvas (Raster)",
            "Light Gray Canvas (Vector)",
            "Navigation (Vector)",
            "OpenStreetMap (Raster)",
            "Oceans"]

        onCurrentTextChanged: {
            // Call C++ invokable function to switch the basemaps
            changeBasemapSample.changeBasemap(currentText);
        }

        onModelChanged: {
            let w = bestWidth;
            for (let i = 0; i < comboBoxBasemap.model.length; ++i) {
                metrics.text = comboBoxBasemap.model[i];
                w = Math.max(w, metrics.width);
            }
            bestWidth = w;
        }

        TextMetrics {
            id: metrics
            font: comboBoxBasemap.font
        }
    }
}
