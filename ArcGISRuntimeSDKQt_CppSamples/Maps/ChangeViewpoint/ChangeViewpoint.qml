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

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

ChangeViewpointSample {
    id: changeViewpointSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    ComboBox {
        id: comboBoxViewpoint
        anchors {
            left: parent.left
            top: parent.top
            margins: 15 * scaleFactor
        }
        width: 175 * scaleFactor
        model: ["Center","Center and scale","Geometry","Geometry and padding","Rotation","Scale 1:5,000,000","Scale 1:10,000,000","Animation"]

        onCurrentTextChanged: {
            // Call C++ invokable function to change the viewpoint
            changeViewpointSample.changeViewpoint(comboBoxViewpoint.currentText);
        }
    }
}
