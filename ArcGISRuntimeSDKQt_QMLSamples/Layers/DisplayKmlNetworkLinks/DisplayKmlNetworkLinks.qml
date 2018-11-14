// [WriteFile Name=DisplayKmlNetworkLinks, Category=Layers]
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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: rootRectangle

    
    property string currentKmlNetworkMessage: "";

    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent
        Scene {
            id: scene
            BasemapImagery {}
            // Center the viewpoint over continental Europe.
            ViewpointCenter {
                Point {
                    x: 8.150526
                    y: 50.472421
                    spatialReference: SpatialReference.createWgs84()
                }
                targetScale: 20000000
            }
            // The layer that will render our KML data.
            KmlLayer {
                // This is a KML resource that references other KML resources over a network.
                dataset:  KmlDataset {
                    url: "https://www.arcgis.com/sharing/rest/content/items/600748d4464442288f6db8a4ba27dc95/data"

                    onKmlNetworkLinkMessageReceived: {
                        currentKmlNetworkMessage = message;
                    }
                }
            }
        }

        MessageButton {
            visible: currentKmlNetworkMessage. length > 0
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 10
            }
            onClicked: {
                messageDialog.open();
            }
        }
    }

    Dialog {
        id: messageDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        width: parent.width * 0.75
        standardButtons: Dialog.Ok
        title: "KML layer message"
        property alias text : textLabel.text
        Text {
            width: parent.width
            id: textLabel
            text: currentKmlNetworkMessage
            wrapMode: Text.WordWrap
        }
        onAccepted: {
            currentKmlNetworkMessage = ""; // Clear the message
        }
        onRejected: onAccepted()
    }
}
