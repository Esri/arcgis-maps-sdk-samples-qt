// [WriteFile Name=TiledLayerRenderMode, Category=Layers]
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // Map view UI presentation at top
    MapView {
        anchors.fill: parent

        Map {
            Basemap {
                ArcGISTiledLayer {
                    id: tiledLayer
                    url: "http://services.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer"
                    renderMode: Enums.RenderModeAesthetics
                }
            }

            ViewpointCenter {
                scale: 86677

                Point {
                    x: -13630860
                    y: 4548447
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
            }
        }
    }

    // switch to select between the two modes
    Switch {
        id: modeSwitch
        anchors {
            left: parent.left
            top: parent.top
            margins: 10 * scaleFactor
        }
        width: 140 * scaleFactor
        height: 25 * scaleFactor

        style: SwitchStyle {

            groove: Rectangle {
                width: modeSwitch.width
                height: modeSwitch.height
                radius: 5
                border {
                    color: "#4D4D4D"
                    width: 1
                }
                color: "lightsteelblue"
            }

            handle: Rectangle {
                width: modeSwitch.width / 2
                height: modeSwitch.height
                radius: 5
                border.color: "#4D4D4D"

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        horizontalCenter: parent.horizontalCenter
                    }
                    text: changeSwitch()
                    renderType: Text.NativeRendering
                    font.pixelSize: 11 * scaleFactor
                }

                // determine text to display on slider
                function changeSwitch() {
                    if (modeSwitch.checked === false)
                        return "Aesthetics";
                    else
                        return "Scale";
                }
            }
        }

        onCheckedChanged: {
            if (modeSwitch.checked === true)
                tiledLayer.renderMode = Enums.RenderModeScale;
            else
                tiledLayer.renderMode = Enums.RenderModeAesthetics;
        }
    }

    // neatline rectangle
    Rectangle {
        anchors.fill: parent
        border {
            color: "black"
            width: 0.5 * scaleFactor
        }
        color: "transparent"
    }
}
