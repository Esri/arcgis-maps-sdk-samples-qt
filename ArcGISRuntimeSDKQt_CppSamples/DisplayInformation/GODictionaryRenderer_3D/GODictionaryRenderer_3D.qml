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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

GODictionaryRenderer_3DSample {
    id: graphicsOverlayDictionaryRenderer_3DSample
    width: 800
    height: 600

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
            margins: 5
        }
        indeterminate: true
    }

    Dialog {
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        visible: text.length > 0
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: errorMessage
            }
            Text {
                id: detailsLabel
                text: "Please consult the README.md"
            }
        }
        onAccepted: errorAccepted()
    }
}
