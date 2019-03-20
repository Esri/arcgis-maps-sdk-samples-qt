// [WriteFile Name=MapReferenceScale, Category=Maps]
// [Legal]
// Copyright 2019 Esri.

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

import QtQuick 2.12
import Esri.ArcGISRuntime 100.5
import QtQuick.Controls 2.12
import Esri.ArcGISExtras 1.1
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

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
                    console.log("Error in portal item: %1(%2)".arg(error.message).arg(error.additionalMessage));
                }
                onLoadStatusChanged: {
                    if (loadStatus === Enums.LoadStatusLoaded) {
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
        height: 150
        width: 200
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.fill: parent
            width: parent.width
            height: parent.height
            color: "#000000"
            opacity: .75
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            ColumnLayout {
                id: referenceScaleLayout
                anchors {
                    fill: parent
                    margins: 5
                }

                Text {
                    color: "#ffffff"
                    text: "Current Map Scale 1:%1".arg(Math.round(mapView.mapScale))
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    verticalAlignment: Text.AlignBottom
                    font {
                        weight: Font.DemiBold
                        pointSize: 9
                    }
                    clip: true
                }

                Text {
                    color: "#ffffff"
                    text: qsTr("Select a new reference scale")
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    verticalAlignment: Text.AlignBottom
                    font {
                        weight: Font.DemiBold
                        pointSize: 9
                    }
                    clip: true
                }

                ComboBox {
                    id: scales
                    font {
                        weight: Font.DemiBold
                        pointSize: 9
                    }
                    Layout.fillWidth: true
                    model: ["1:500000","1:250000","1:100000","1:50000"]
                    clip: true
                    Component.onCompleted: applyReferenceScaleToMap()
                    onActivated: applyReferenceScaleToMap()
                }

                Button {
                    text: qsTr("Set Map Scale to Reference Scale")
                    font {
                        weight: Font.DemiBold
                        pointSize: 8
                    }
                    Layout.fillWidth: true
                    clip: true
                    onClicked: mapView.setViewpointScale(referenceScales[scales.currentIndex])
                }
            }
        }
    }

    Rectangle {
        id: operationalLayersList
        anchors {
            margins: 10
            right: parent.right
            top: parent.top
        }
        height: 235
        width: 130
        color: "transparent"
        clip: true

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }


        Rectangle {
            anchors.fill: parent
            width: parent.width
            height: parent.height
            color: "#000000"
            opacity: .75
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }
            clip: true

            ColumnLayout {
                id: operationalLayersLayout
                anchors {
                    fill: parent
                    margins: 2
                }
                anchors {
                    margins: 10
                    right: parent.right
                    top: parent.top
                }
                spacing: 0
                clip: true

                Text {
                    text: qsTr("Apply Reference Scale")
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    font {
                        weight: Font.DemiBold
                        pointSize: 9
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
                    Layout.fillWidth: true
                    clip: true
                    spacing: 5.5

                    // Assign the model to the list model of operational layers
                    model: featureLayerModel

                    // Assign the delegate to display text next to checkbox as a row
                    delegate: Item {
                        width: parent.width
                        height: 25

                        CheckBox {
                            id: featureLayerBox
                            Text {
                                id: featureLayerText
                                text: name
                                anchors.horizontalCenterOffset: featureLayerBox.width + 15
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                wrapMode: Text.WordWrap
                                font.pointSize: 9
                                color: "#ffffff"
                                clip: true
                            }
                            checked: true
                            onCheckStateChanged: featureLayerScaleSymbols(name,featureLayerBox.checked)
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
    function featureLayerScaleSymbols(layerName, checkedStatus) {
        featureLayerModel.forEach(function(lyr){
            if(layerName === lyr.name){
                lyr.scaleSymbols = checkedStatus
            }
        })
    }
}
