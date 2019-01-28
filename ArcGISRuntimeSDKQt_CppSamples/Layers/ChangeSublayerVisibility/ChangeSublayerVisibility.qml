// [WriteFile Name=ChangeSublayerVisibility, Category=Layers]
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
import QtQuick.Window 2.2
import Esri.Samples 1.0

ChangeSublayerVisibilitySample {
    id: changeSublayerVisibilitySample
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // Create a window to display the sublayers
    Rectangle {
        id: layerVisibilityRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 150
        width: 150
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
            opacity: .9
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

                Text {
                    width: parent.width
                    text: "Sublayers"
                    wrapMode: Text.WordWrap
                    clip: true
                    font {
                        pointSize: 14
                        bold: true
                    }
                }

                // Create a list view to display the items
                ListView {
                    id: layerVisibilityListView
                    anchors.margins: 10
                    width: parent.width
                    height: parent.height
                    clip: true

                    // Assign the model to the list model of sublayers
                    model: changeSublayerVisibilitySample.sublayerModel

                    // Assign the delegate to the delegate created above
                    delegate: Item {
                        id: layerVisibilityDelegate
                        width: parent.width
                        height: 35

                        Row {
                            spacing: 5
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                width: 75
                                text: name
                                wrapMode: Text.WordWrap
                                font.pointSize: 14
                            }

                            Switch {
                                anchors.verticalCenter: parent.verticalCenter
                                checked: sublayerVisible

                                onCheckedChanged: {
                                    sublayerVisible = checked;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
