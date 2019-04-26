// [WriteFile Name=DisplayDrawingStatus, Category=Maps]
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
import QtGraphicalEffects 1.0
import Esri.Samples 1.0

DisplayDrawingStatusSample {
    id: displayDrawingStatusSample
    clip: true
    width: 800
    height: 600

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // dim window if map is drawing
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: displayDrawingStatusSample.mapDrawing

        RadialGradient {
            anchors.fill: parent
            opacity: 0.15
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        // pop up to show if MapView is drawing
        Rectangle {
            anchors.centerIn: parent
            width: 100
            height: 100
            radius: 3
            opacity: 0.85
            color: "#E0E0E0"
            border.color: "black"

            Column {
                anchors {
                    centerIn: parent
                    topMargin: 5
                }
                spacing: 5

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 60
                    running: true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font {
                        weight: Font.Black
                        pixelSize: 12
                    }
                    height: 20
                    horizontalAlignment: Text.AlignHCenter
                    text: "Drawing..."
                }
            }
        }
    }
}
