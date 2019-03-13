// [WriteFile Name=MapReferenceScale, Category=Maps]
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
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import QtQuick.Layouts 1.12

Item {

    property var referenceScales: [500000,250000,100000,50000]

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    Rectangle {
        id: referenceScaleRect
        anchors {
            margins: 10
            left: parent.left
            top: parent.top
        }
        height: 180
        width: 200
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.fill: parent
            width: referenceScaleRect.width
            height: referenceScaleRect.height
            color: "#000000"
            opacity: .75
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            ColumnLayout {
                id: referenceScaleLayout
                spacing: 1
                anchors {
                    fill: parent
                    margins: 5
                }

                Text {
                    color: "#ffffff"
                    text: qsTr("Current Map Scale 1:" + Math.round(model.mapScale))
                    font {
                        weight: Font.DemiBold
                        pixelSize: 11
                    }
                    clip: true
                }

                Text {
                    color: "#ffffff"
                    text: qsTr("Select a new reference scale")
                    font {
                        weight: Font.DemiBold
                        pixelSize: 11
                    }
                    clip: true
                }

                ComboBox {
                    id: scales
                    font {
                        weight: Font.DemiBold
                        pixelSize: 11
                    }
                    Layout.fillWidth: true
                    model: ["1:500000","1:250000","1:100000","1:50000"]
                    clip: true
                }

                Button {
                    id: button
                    text: qsTr("Set Map Scale to Reference Scale")
                    font {
                        weight: Font.DemiBold
                        pixelSize: 11
                    }
                    Layout.fillWidth: true
                    clip: true
                }
            }
        }
    }

    Rectangle {
        id: operationalLayersList
        anchors {
            margins: 5
            right: parent.right
            top: parent.top
        }
        height: 200
        width: 125
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.fill: parent
            width: operationalLayersList.width
            height: operationalLayersList.height
            color: "#000000"
            opacity: .75
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            ColumnLayout {
                id: operationalLayersLayout
                anchors {
                    fill: parent
                    margins: 10
                }

                Text {
                    text: qsTr("Apply Reference Scale")
                    font {
                        weight: Font.DemiBold
                        pixelSize: 11
                    }
                    color: "#ffffff"
                    clip: true
                }

                // Create a list view to display the items
                ListView {
                    id: layerVisibilityListView
                    anchors.margins: 10
                    width: parent.width
                    height: parent.height
                    clip: true

                    //assign model
                    model: model.layerInfoListModel

                    // Assign the delegate to the delegate created above
                    delegate: Item {
                        id: layerVisibilityDelegate
                        width: parent.width
                        height: 25

                        Row {
                            id: layerRows
                            spacing: 2

                            CheckBox {
                                id: featureLayerBox
                                anchors.verticalCenter: parent.verticalCenter
                                height: 15
                                width: 15
                                clip: true
                                checked: true
                            }

                            Text {
                                id: featureLayerText
                                anchors.verticalCenter: parent.verticalCenter
                                text: name
                                wrapMode: Text.WordWrap
                                font.pixelSize: 11
                                color: "#ffffff"
                                clip: true
                            }
                        }
                    }
                }
            }
        }
    }
    // Declare the C++ instance which creates the scene etc. and supply the view
    MapReferenceScaleSample {
        id: model
        mapView: view
    }
}
