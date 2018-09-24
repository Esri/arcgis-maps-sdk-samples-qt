// [WriteFile Name=DisplayKml, Category=Layers]
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
import QtQuick.Controls 1.4
import QtQuick.Window 2.3
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

DisplayKmlSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/kml/"

    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    ComboBox {
        anchors {
            right: parent.right
            top: parent.top
            margins: 5 * scaleFactor
        }

        model: ["URL", "Local file", "Portal Item"]

        onCurrentIndexChanged: {
            // create the Layer
            if (currentIndex === 0)
                createFromUrl();
            else if (currentIndex === 1)
                createFromFile();
            else
                createFromPortalItem();
        }
    }
}
