// [WriteFile Name=CreateSymbolStylesFromWebStyles, Category=DisplayInformation]
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

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {
    id: window

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateSymbolStylesFromWebStylesSample {
        id: sample
        mapView: view
    }

    Rectangle {
        id: legendRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 545
        width: 175
        color: "lightgrey"
        opacity: 0.9
        clip: true
        border {
            color: "darkgrey"
            width: 1
        }

        // Catch mouse signals so they don't propagate to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        Column {
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 2

            Row {
                spacing: 55

                Text {
                    text: qsTr("Legend")
                    font {
                        pixelSize: 18
                        bold: true
                    }
                }
            }

            // Create a list view to display the legend
            ListView {
                id: legendListView
                anchors.margins: 10
                model: sample.legendInfoListModel
                width: parent.width * .9
                height: parent.height * .9
                clip: true

                // Create delegate to display the name with an image
                delegate: Item {
                    id: legendItem
                    width: 175
                    height: 40
                    clip: true

                    Row {
                        spacing: 5
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 25
                            height: width
                            source: symbolUrl
                        }

                        Text {
                            id: symbolText
                            anchors.verticalCenter: parent.verticalCenter
                            width: 110
                            text: name
                            wrapMode: Text.Wrap
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }
}
