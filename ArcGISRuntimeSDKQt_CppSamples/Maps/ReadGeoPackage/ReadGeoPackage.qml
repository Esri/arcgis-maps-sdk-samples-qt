// [WriteFile Name=ReadGeoPackage, Category=Maps]
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
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import Esri.ArcGISExtras 1.1
import Esri.Samples 1.0

ReadGeoPackageSample {
    id: root
    clip: true
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"
    }

    // Create the layer selection menu
    Rectangle {
        id: layerVisibilityRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 210
        width: 225
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
                    margins: 10
                }
                clip: true

                Text {
                    width: parent.width
                    text: "GeoPackage Layers"
                    wrapMode: Text.WordWrap
                    clip:true
                    font {
                        pixelSize: 14
                        bold: true
                    }
                }

                // Populate the menu with the layers from the GeoPackage
                ListView {
                    id: layerVisibilityListView
                    anchors.margins: 20
                    width: parent.width
                    height: parent.height
                    clip: true
                    model: root.layerList
                    delegate: Item {
                        id: visibilityDelegate
                        width: parent.width
                        height: 35
                        Row {
                            spacing: 5
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                width: 150
                                text:  modelData.name
                                wrapMode: Text.WordWrap
                                font.pixelSize: 14
                            }

                            Switch {
                                onCheckedChanged: {
                                    root.addOrShowLayer(index, checked);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

