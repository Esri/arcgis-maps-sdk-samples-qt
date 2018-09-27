// [WriteFile Name=DisplayKMLNetworkLinks, Category=Layers]
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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.3
import Esri.Samples 1.0

DisplayKMLNetworkLinksSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    MessageDialog {
        id: messageDialog
        title: "KML layer message"
        text: currentKmlNetworkMessage
        onAccepted: {
            currentKmlNetworkMessage = ""; // Clear the message
        }
    }

    SceneView {
        id: sceneView
        objectName: "sceneView"
        anchors.fill: parent

        MessageButton {
            visible: currentKmlNetworkMessage.length > 0
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 10 * scaleFactor
            }
            onClicked: {
                messageDialog.open();
            }
        }
    }
}
