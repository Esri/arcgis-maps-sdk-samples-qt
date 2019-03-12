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
import Esri.ArcGISRuntime 100.5
import QtQuick.Controls 2.2
import Esri.ArcGISExtras 1.1
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var referenceScales: [500000,250000,100000,50000]

    MapView{
        id: mapView
        anchors.fill: parent

        Portal {
            id: agol
        }

        PortalItem {
            id: itemFromId
            portal: agol
            itemId: "3953413f3bd34e53a42bf70f2937a408"
        }

        FeatureLayer {
            id: featLay
            ServiceFeatureTable {
                initItem: itemFromId
                initLayerId: "3953413f3bd34e53a42bf70f2937a408"
            }
        }

        Map {
            id: webmapFromItem
            item: itemFromId

            onLoadStatusChanged: {
                itemFromId.load()
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;
            }
        }
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
                    text: qsTr("Current Map Scale 1:" + Math.round(mapView.mapScale))
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

                    onActivated: {
                        //webmapFromItem.referenceScale = referenceScales[scales.currentIndex]
                        //apply to selected featurelayers
                        applyReferenceScaleToSelected()
                    }
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

                    onClicked: {
                        mapView.setViewpointScale(referenceScales[scales.currentIndex])
                    }
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

                    // Assign the model to the list model of sublayers
                    model: webmapFromItem.operationalLayers

                    // Assign the delegate to the delegate created above
                    delegate: Item {
                        id: layerVisibilityDelegate
                        width: parent.width
                        height: 25

                        Row {
                            spacing: 2

                            CheckBox {
                                id: featureLayerBox
                                anchors.verticalCenter: parent.verticalCenter
                                height: 15
                                checked: true
                                width: 15
                                clip: true
                                onCheckStateChanged: {
                                    console.log(featureLayerText.text)
                                    console.log(layerType)
                                    if( featureLayerBox.checked ){
                                        layerVisible = true
                                    } else {
                                        layerVisible = false
                                    }
                                }
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

    function applyReferenceScaleToSelected() {


        webmapFromItem.referenceScale = referenceScales[scales.currentIndex]
    }

}



