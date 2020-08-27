// [WriteFile Name=MapRotation, Category=Maps]
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
import Esri.ArcGISRuntime 100.9

Rectangle {
    width: 800
    height: 600    

    // Map view UI presentation at top
    MapView {
        id: mv

        anchors.fill: parent

        Map {
            BasemapStreets {}
            initialViewpoint: vc
        }

        ViewpointCenter {
            id: vc
            center: Point {
                x: -11070722
                y: 4658720
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
            targetScale: 55151630
        }

        // Slider UI presentation at bottom
        Rectangle {
            anchors {
                bottom: mv.attributionTop
                horizontalCenter: parent.horizontalCenter
            }

            width: childrenRect.width
            height: childrenRect.height
            radius: 10
            opacity: 0.8

            // sliderCombo: A slider and text for its value
            Row {
                id: sliderCombo
                spacing: 5

                Slider {
                    id: slider1
                    opacity: 0.7
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    // Slider controls degrees of rotation
                    from: 0.0
                    to: 360.0
                    onPressedChanged: {
                        // Call C++ invokable function to change the rotation
                        // of the map view
                        mv.setViewpointRotation(value);
                    }
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        margins: 5
                    }
                    horizontalAlignment: TextInput.AlignHCenter
                    font.pixelSize: 20
                    text: slider1.value.toPrecision(3);
                }
            }
        }
    }
}
