// [WriteFile Name=BuildLegend, Category=DisplayInformation]
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
import Esri.Samples 1.0

BuildLegendSample {
    id: showLegendSample
    width: 800
    height: 600    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // Create outter rectangle for the legend
    Rectangle {
        id: legendRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        property bool expanded: true
        height: 200
        width: 175
        color: "lightgrey"
        opacity: 0.95
        radius: 10
        clip: true
        border {
            color: "darkgrey"
            width: 1
        }

        // Animate the expand and collapse of the legend
        Behavior on height {
            SpringAnimation {
                spring: 3
                damping: .4
            }
        }

        // Catch mouse signals so they don't propagate to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        // Create UI for the user to select the layer to display
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

                // Legend icon to allow expanding and collapsing
                Image {
                    source: legendRect.expanded ? "qrc:/Samples/DisplayInformation/BuildLegend/ic_menu_legendpopover_light_d.png" : "qrc:/Samples/DisplayInformation/BuildLegend/ic_menu_legendpopover_light.png"
                    width: 28
                    height: width

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (legendRect.expanded) {
                                legendRect.height = 40;
                                legendRect.expanded = false;
                            } else {
                                legendRect.height = 200;
                                legendRect.expanded = true;
                            }
                        }
                    }
                }
            }

            // Create a list view to display the legend
            ListView {
                id: legendListView
                anchors.margins: 10
                model: showLegendSample.legendInfoListModel
                width: 165
                height: 150
                clip: true

                // Create delegate to display the name with an image
                delegate: Item {
                    width: parent.width
                    height: 35
                    clip: true

                    Row {
                        spacing: 5
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 24
                            height: width
                            source: symbolUrl
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 125
                            text: name
                            wrapMode: Text.WordWrap
                            font.pixelSize: 12
                        }

                    }
                }

                section {
                    property: "layerName"
                    criteria: ViewSection.FullString
                    labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
                    delegate: Rectangle {
                        width: 180
                        height: childrenRect.height
                        color: "lightsteelblue"

                        Text {
                            text: section
                            font.bold: true
                            font.pixelSize: 13
                        }
                    }
                }
            }
        }
    }
}


