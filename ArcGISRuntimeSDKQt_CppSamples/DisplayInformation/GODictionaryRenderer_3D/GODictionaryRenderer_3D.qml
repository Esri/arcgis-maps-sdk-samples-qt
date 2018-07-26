// [WriteFile Name=GraphicsOverlayDictionaryRenderer_3D, Category=DisplayInformation]
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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import Esri.ArcGISExtras 1.1
import Esri.Samples 1.0

GODictionaryRenderer_3DSample {
    id: graphicsOverlayDictionaryRenderer_3DSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"

    onGraphicsLoaded: progressBar_loading.visible = false;

    // Create SceneView here, and create its Scene and GraphicsOverlay in C++ code
    SceneView {
        objectName: "sceneView"
        anchors.fill: parent
    }

    ProgressBar {
        id: progressBar_loading
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 5 * scaleFactor
        }
        indeterminate: true
    }

    MessageDialog {
        visible: text.length > 0
        text: errorMessage
        informativeText: "Please consult the README.md"
    }
}
