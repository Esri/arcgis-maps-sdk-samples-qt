// [WriteFile Name=DisplayLayerViewDrawStatus, Category=Maps]
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
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import Esri.Samples 1.0

DisplayLayerViewDrawStatusSample {
    id: displayLayerView
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter

        }
        border.color: "black"
        radius: 10
        height: 150 * scaleFactor
        width: 230 * scaleFactor

        ListView {
            height: childrenRect.height
            id: listView
            anchors {
                fill: parent
                margins: 25 * scaleFactor
            }
            model: statusModel
            opacity: 0.95
            spacing: 10 * scaleFactor
            delegate: Item {
                width: parent.width
                height: childrenRect.height
                Text {
                    text: name
                    horizontalAlignment: Text.AlignLeft
                    anchors.left: parent.left
                }
                Text {
                    text: status
                    color: "steelblue"
                    horizontalAlignment: Text.AlignRight
                    anchors.right: parent.right
                }
            }
        }
    }
}
