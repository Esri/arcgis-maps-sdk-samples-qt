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
    width: 500
    height: 500

    property var referenceScales: [500000,250000,100000,50000]
    property string webMapId: "3953413f3bd34e53a42bf70f2937a408"
    property LayerListModel featureLayerModel

    MapView{
        id: mapView
        anchors.fill: parent

        // Create a Map from a Portal Item
        Map {
            id: map

            PortalItem {
                id: mapPortalItem
                portal: portal
                itemId: webMapId
                onErrorChanged: {
                        console.log("Error in portal item: " + error.message + "(" + error.additionalMessage + ")");
                }
                onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            console.log("Item title: " + title);
                            //load operational layers into listmodel
                            featureLayerModel = map.operationalLayers
                        }
                }
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
                        applyReferenceScaleToMap()
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
                    model: featureLayerModel

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
                                onCheckStateChanged: {
                                    updateFeatureLayerScaleSymbols(name,featureLayerBox.checked)
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

    function applyReferenceScaleToMap() {
        map.referenceScale = referenceScales[scales.currentIndex]
    }

    //pass in layers name and checked status to update featurelayer.ScaleSymbols property accordingly
    function updateFeatureLayerScaleSymbols(layerName, checkedStatus) {
        for(var i = 0; i < featureLayerModel.count; i++){
            var featureLayer = featureLayerModel.get(i)
            if(layerName === featureLayer.name){
                if(checkedStatus){
                    featureLayer.scaleSymbols = true
                } else {
                    featureLayer.scaleSymbols = false
                }
            }
        }
    }
}



