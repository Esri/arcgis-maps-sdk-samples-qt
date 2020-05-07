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
import Esri.ArcGISRuntime 100.9
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property var referenceScales: [500000, 250000, 100000, 50000]
    readonly property string webMapId: "3953413f3bd34e53a42bf70f2937a408"
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
                        featureLayerModel = map.operationalLayers;
                    }
                }
            }
        }
    }

    Rectangle {
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .75
        radius: 5

        ColumnLayout {
            Text {
                color: "#ffffff"
                text: "Current Map Scale 1:%1".arg(Math.round(mapView.mapScale))
                Layout.fillWidth: true
                Layout.margins: 3
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
            }

            Text {
                color: "#ffffff"
                text: qsTr("Select a new reference scale")
                Layout.fillWidth: true
                Layout.margins: 3
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
            }

            ComboBox {
                id: scales
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
                Layout.margins: 3
                Layout.fillWidth: true
                model: ["1:500000","1:250000","1:100000","1:50000"]
                Component.onCompleted: applyReferenceScaleToMap();
                onActivated: applyReferenceScaleToMap();
            }

            Button {
                text: qsTr("Set Map Scale to Reference Scale")
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
                Layout.margins: 3
                Layout.fillWidth: true
                onClicked: mapView.setViewpointScale(referenceScales[scales.currentIndex]);
            }
        }
    }

    Rectangle {
        anchors {
            margins: 5
            right: parent.right
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .75
        radius: 5

        ColumnLayout {
            Text {
                text: qsTr("Apply Reference Scale")
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.margins: 2
                font {
                    weight: Font.DemiBold
                    pointSize: 10
                }
                color: "#ffffff"
            }

            Repeater {
                id: featureLayerRepeater
                // Assign the model to the list model of operational layers
                model: featureLayerModel
                width: childrenRect.width
                height: childrenRect.height

                // Assign the delegate to display text next to checkbox as a row
                delegate: Row {
                    CheckBox {
                        id: featureLayerBox
                        checked: true
                        onCheckStateChanged: featureLayerScaleSymbols(name,featureLayerBox.checked);
                    }
                    Text {
                        id: featureLayerText
                        text: name
                        height: featureLayerBox.height
                        verticalAlignment: Text.AlignVCenter
                        font.pointSize: 10
                        color: "#ffffff"
                    }
                }
            }
        }
    }

    function applyReferenceScaleToMap() {
        map.referenceScale = referenceScales[scales.currentIndex];
    }

    //pass in layers name and checked status to update featurelayer.ScaleSymbols property accordingly
    function featureLayerScaleSymbols(layerName, checkedStatus) {
        featureLayerModel.forEach(lyr => {
            if(layerName === lyr.name){
                lyr.scaleSymbols = checkedStatus;
            }
        })
    }
}
