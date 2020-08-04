// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property string portalItemId: "b8f4033069f141729ffb298b7418b653"
    property var statusStringList: []
    property var warningMessage: ""



    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            FeatureLayer {
                id: featureLayer
                serviceLayerIdAsInt: 0
                minScale: 400000000
                maxScale: 400000000 / 10
                visible: loadLayerButton.text === qsTr("Hide Layer") ? true : false

                PortalItem {
                    id: portalItem
                    itemId: portalItemId
                }

                // once loaded set the viewpoint
                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    mapView.map.operationalLayers.append(featureLayer);
                    mapView.setViewpointCenter(viewPoint);
                    mapView.setViewpointScale(40000000.0);
                }
            }
        }

        Dialog {
            id: warningDialog
            anchors.centerIn: parent
            standardButtons: Dialog.Ok
            visible: warningMessage !== "" ? true : false
            Text {
                id: warningText
                text: warningMessage;
            }
        }

        onLayerViewStateChanged: {
            // only update list if the layer is the feature layer.
            if (layer.name != featureLayer.name)
                return;

            // clear list for new view state(s).
            statusStringList = [];

            if (layerViewState.statusFlags & Enums.LayerViewStatusActive) {
                statusStringList.push("Active");
            }
            if (layerViewState.statusFlags & Enums.LayerViewStatusError) {
                statusStringList.push("Error");
            }
            if (layerViewState.statusFlags & Enums.LayerViewStatusLoading) {
                statusStringList.push("Loading");
            }
            if (layerViewState.statusFlags & Enums.LayerViewStatusNotVisible) {
                statusStringList.push("NotVisible");
            }
            if (layerViewState.statusFlags & Enums.LayerViewStatusOutOfScale) {
                statusStringList.push("OutOfScale");
            }
            if (layerViewState.statusFlags & Enums.LayerViewStatusWarning) {
                statusStringList.push("Warning");
                warningMessage = qsTr("Warning message: %1".arg(layerViewState.loadError.message));
            }

            layerViewStatusRepeater.model = statusStringList;
        }

        Rectangle {
            id: controlRect
            anchors {
                bottom: loadLayerButton.top
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#1976D2"
            radius: 3

            MouseArea {
                width: controlLayout.childrenRect.width
                height: controlLayout.childrenRect.height
                onClicked: mouse.accepted = true;
                onWheel: wheel.accepted = true;
            }

            ColumnLayout{
                id: controlLayout

                Text {
                    id: textHeader
                    text: qsTr("Current view status:")
                    color: "white"
                    Layout.alignment: Qt.AlignHCenter
                }

                Column {
                    id: column
                    Layout.alignment: Qt.AlignHCenter
                    Repeater {
                        id: layerViewStatusRepeater
                        Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            Text {
                                text: modelData
                                color: "white"
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: loadLayerButton
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            text: qsTr("Load Layer")
            enabled: featureLayer.loadStatus !== Enums.LoadStatusLoading ? true : false
            clip: true
            onClicked: {
                if (text === qsTr("Load Layer")) {
                    // load a feature layer from a portal item
                    featureLayer.load();
                    text = qsTr("Hide Layer");
                }
                else if (text === qsTr("Hide Layer"))
                    text = qsTr("Show Layer");
                else if (text === qsTr("Show Layer"))
                    text = qsTr("Hide Layer");
            }
        }
    }

    Point {
        id: viewPoint
        x: -11000000
        y: 4500000
        spatialReference: Factory.SpatialReference.createWebMercator();
    }
}
