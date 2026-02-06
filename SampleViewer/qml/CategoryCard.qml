// [Legal]
// Copyright 2022 Esri.

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
import QtQuick.Effects
import Esri.ArcGISRuntimeSamples
import Calcite

ItemDelegate {
    id: card
    width: 105
    height: 105
    clip: false

    property string thumbnailUrl
    property string displayName
    property string backgroundThumbnailUrl

    // Hover/press scale effect with z-ordering
    scale: pressed ? 0.97 : (hovered ? 1.03 : 1.0)
    z: hovered ? 1 : 0
    Behavior on scale {
        NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
    }

    background: Item {
        Item {
            anchors.fill: parent
            visible: true

            Image {
                id: categoryImg
                anchors.fill: parent
                source: backgroundThumbnailUrl
                fillMode: Image.PreserveAspectCrop
            }

            Rectangle {
                anchors.fill: parent
                radius: 10
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#25000000" }
                    GradientStop { position: 0.5; color: "#65000000" }
                    GradientStop { position: 1.0; color: "#CC000000" }
                }
            }
        }

        // Border on top
        Rectangle {
            anchors.fill: parent
            anchors.margins: (card.hovered || card.pressed) ? -1 : 0
            radius: (card.hovered || card.pressed) ? 11 : 10
            color: "transparent"
            border.width: (card.hovered || card.pressed) ? 4 : 1
            border.color: (card.hovered || card.pressed) ? Calcite.brand : Calcite.border3
            Behavior on anchors.margins {
                NumberAnimation { duration: 150 }
            }
            Behavior on radius {
                NumberAnimation { duration: 150 }
            }
            Behavior on border.width {
                NumberAnimation { duration: 150 }
            }
            Behavior on border.color {
                ColorAnimation { duration: 150 }
            }
        }
    }

    contentItem: Item {
        anchors.fill: parent

        Column {
            anchors.centerIn: parent
            width: parent.width
            spacing: 2

            Rectangle {
                id: iconCircle
                anchors.horizontalCenter: parent.horizontalCenter
                width: 42
                height: width
                radius: width / 2
                color: Calcite.textInverse
                border.width: Calcite.theme !== Calcite.Theme.Light ? 1 : 0
                border.color: Calcite.border1

                Image {
                    id: thumbnailImage
                    anchors.centerIn: parent
                    width: 24
                    height: width
                    source: thumbnailUrl
                    clip: true

                    layer.enabled: true
                    layer.smooth: true
                    layer.effect: MultiEffect {
                        anchors.fill: thumbnailImage
                        source: thumbnailImage
                        colorization: 1.0
                        brightness: 1.0
                        colorizationColor: Calcite.text1
                        visible: true
                    }
                }
            }

            Text {
                width: parent.width
                antialiasing: true
                text: displayName
                visible: drawer.visible
                color: "white"
                font {
                    capitalization: Font.AllUppercase
                    pixelSize: 11
                    weight: Font.DemiBold
                    family: fontFamily
                }
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    onClicked: {
        sampleListView.currentCategory = displayName;
        SampleManager.currentCategory = SampleManager.categories.get(index);
        stackView.push(sampleListView);
    }
}
