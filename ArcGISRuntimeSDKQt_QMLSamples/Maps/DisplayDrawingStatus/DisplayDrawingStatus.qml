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

import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime
import Esri.ArcGISExtras

Rectangle {
    clip: true
    width: 800
    height: 600



    // create MapView
    MapView {
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // create map using topographic basemap
        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }

            // create FeatureLayer using a service URL
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"
                }
            }

            // set initial viewpoint
            ViewpointExtent {
                Envelope {
                    xMin: -13639984
                    yMin: 4537387
                    xMax: -13606734
                    yMax: 4558866
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        // make Drawing Window visible if map is drawing. Not visible if drawing completed
        onDrawStatusChanged: {
            drawStatus === Enums.DrawStatusInProgress ? mapDrawingWindow.visible = true : mapDrawingWindow.visible = false;
        }
    }

    Rectangle {
        id: mapDrawingWindow
        anchors.fill: parent
        color: "transparent"

        Rectangle {
            anchors.fill: parent
            color: "#60000000"
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
                anchors.centerIn: parent
                topPadding: 5
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
