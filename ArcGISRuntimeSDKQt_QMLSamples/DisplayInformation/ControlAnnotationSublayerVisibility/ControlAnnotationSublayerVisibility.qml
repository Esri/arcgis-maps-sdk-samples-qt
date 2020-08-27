// [WriteFile Name=ControlAnnotationSublayerVisibility, Category=DisplayInformation]
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

import QtQuick 2.6
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.6
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data/mmpk/"
    property AnnotationSublayer openLayer
    property AnnotationSublayer closedLayer

    MapView {
        id: mapView
        anchors.fill: parent

        Rectangle {
            id: checkBoxBackground
            anchors {
                left: parent.left
                top: parent.top
                margins: 2
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "white"
            opacity: .75
            radius: 5

            ColumnLayout {
                spacing: 0
                Row {
                    CheckBox {
                        id: openBox
                        checked: true
                        onCheckStateChanged: {
                            if (openLayer) {
                                openLayer.visible = checked;
                            }
                        }
                    }

                    Text {
                        id: openBoxText
                        anchors.verticalCenter: openBox.verticalCenter
                        color: scale.color
                    }
                }

                Row {
                    CheckBox {
                        id: closedBox
                        checked: true
                        onCheckStateChanged: {
                            if (closedLayer) {
                                closedLayer.visible = checked;
                            }
                        }
                    }

                    Text {
                        id: closedBoxText
                        anchors.verticalCenter: closedBox.verticalCenter
                    }
                }
            }
        }

        Rectangle {
            id: currentScale
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: parent.horizontalCenter
            }
            width: childrenRect.width
            height: childrenRect.height

            Text {
                id: scale
                text: "Current map scale: 1:%1".arg(Math.round(mapView.mapScale))
                color: "grey"
                padding: 2
            }
        }

        onMapScaleChanged: {
            if (openLayer) {
                if (openLayer.isVisibleAtScale(mapView.mapScale)) {
                    scale.color = "black";
                } else {
                    scale.color = "grey";
                }
            }
        }
    }

    Component.onCompleted: {
        mmpk.load();
    }

    MobileMapPackage {
        id: mmpk
        path: dataPath + "GasDeviceAnno.mmpk"

        // wait for the mobile map package to load
        onLoadStatusChanged: {
            // only proceed once the map package is loaded
            if (loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }

            if (mmpk.maps.length < 1) {
                return;
            }

            // set the map view's map to the first map in the mobile map package
            mapView.map = mmpk.maps[0];
            const layers = mapView.map.operationalLayers;
            layers.forEach(layer => {
                if (layer.layerType === Enums.LayerTypeAnnotationLayer) {
                    layer.loadStatusChanged.connect(()=> {
                        if (layer.loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        closedLayer = layer.subLayerContents[0];
                        openLayer = layer.subLayerContents[1];
                        closedBoxText.text = closedLayer.name;
                        openBoxText.text = "%1 (1:%2 - 1:%3)".arg(openLayer.name).arg(openLayer.maxScale).arg(openLayer.minScale);
                    });
                    layer.load()
                }
            });
        }

        onErrorChanged: {
            console.log("Mobile Map Package Error: %1 %2".arg(error.message).arg(error.additionalMessage));
        }
    }
}
