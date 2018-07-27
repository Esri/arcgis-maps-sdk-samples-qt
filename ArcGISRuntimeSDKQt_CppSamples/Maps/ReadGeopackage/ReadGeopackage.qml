// [WriteFile Name=ReadGeopackage, Category=Maps]
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
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1
import Esri.Samples 1.0

ReadGeopackageSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    onLayerNamesListChanged: layerVisibilityListView.model = layerNamesList;

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        Component.onCompleted: {
            layerVisibilityListView.model = ReadGeopackage.layerNamesList;
        }
    }

    // Create the layer selection menu
    Rectangle {
        id: layerVisibilityRect
        anchors {
            margins: 10 * scaleFactor
            left: parent.left
            top: parent.top
        }
        height: 210 * scaleFactor
        width: 225 * scaleFactor
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.fill: parent
            width: layerVisibilityRect.width
            height: layerVisibilityRect.height
            color: "lightgrey"
            opacity: 0.9
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                clip: true

                Text {
                    width: parent.width
                    text: "Geopackage Layers"
                    wrapMode: Text.WordWrap
                    clip:true
                    font {
                        pixelSize: 14 * scaleFactor
                        bold: true
                    }
                }

                // Populate the menu with the layers from the geopackage
                ListView {
                    id: layerVisibilityListView
                    anchors.margins: 20 * scaleFactor
                    width: parent.width
                    height: parent.height
                    clip: true
                    model: rootRectangle.layerNamesList
                    delegate: Item {
                        id: visibilityDelegate
                        width: parent.width
                        height: 35 * scaleFactor
                        Row {
                            spacing: 5
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                width: 150 * scaleFactor
                                text:  modelData["name"]
                                wrapMode: Text.WordWrap
                                font.pixelSize: 14 * scaleFactor
                            }

                            Switch {
                                onCheckedChanged: {
                                    rootRectangle.addOrShowLayer(index, checked);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

