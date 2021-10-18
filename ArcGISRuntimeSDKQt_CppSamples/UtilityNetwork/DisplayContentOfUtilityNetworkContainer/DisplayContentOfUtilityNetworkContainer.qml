// [WriteFile Name=DisplayContentOfUtilityNetworkContainer, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {
    id: item1

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplayContentOfUtilityNetworkContainerSample {
        id: model
        mapView: view
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
        focus: true

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Create outter rectangle for the legend
    Rectangle {
        id: containerViewOverlay
        anchors.fill: parent

        // Prevent MapView interaction
        MouseArea {
            anchors.fill: parent
        }
        ScrollView {
            anchors.fill: parent
        }

        color: "transparent"
        border.color: "transparent"

        visible: model.showContainerView
        onVisibleChanged: {
            if (containerViewOverlay.visible)
                containerViewOverlay.focus = true;
            else
                view.focus = true;
        }

        Control {
            id: legendBox
            anchors {
                top: parent.top
                left: parent.left
                margins: 20
            }
            background: Rectangle {
                border.color: "black"
                border.width: 1
            }
            padding: 5

            contentItem: GridLayout {
                id: grid
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 2
                Layout.fillWidth: true

                Image {
                    id: attachmentImage
                    source: model.attachmentSymbolUrl
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    id: attachmentLabel
                    text: "Attachment"
                }

                Image {
                    id: connectivityImage
                    source: model.connectivitySymbolUrl
                }
                Label {
                    id: connectivityLabel
                    text: "Connectivity"
                }

                Image {
                    id: boundingBoxSymbol
                    source: model.boundingBoxSymbolUrl
                }
                Label {
                    id: boundingBoxLabel
                    text: "Bounding box"
                }
            }
        }

        Button {
            id: containerCloseButton
            anchors {
                bottom: parent.bottom
                bottomMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
            background: Rectangle {
                color: "white"
                border.color: "black"
            }
            text: "Close container view"
            font.pointSize: 16
            onClicked: {
                model.showContainerView = false;
            }
        }
    }

    Control {
        id: messageBoxPopup
        anchors {
            centerIn: parent
        }
        padding: 10
        width: Math.max(messageBoxText.width, closeMessage.width) + (padding * 2)
        height: messageBoxText.height + closeMessage.height + (messageBoxPopup.padding * 3)
        background: Rectangle {
            color: "white"
            border.color: "black"
        }

        visible: model.messageBoxText !== ""

        Text {
            id: messageBoxText
            anchors {
                top: parent.top
                topMargin: messageBoxPopup.padding
                horizontalCenter: parent.horizontalCenter
            }
            text: model.messageBoxText
        }

        Button {
            id: closeMessage
            anchors {
                bottom: parent.bottom
                bottomMargin: messageBoxPopup.padding
                horizontalCenter: parent.horizontalCenter
            }
            text: "Close"
            onClicked: model.messageBoxText = "";
        }
    }
}
