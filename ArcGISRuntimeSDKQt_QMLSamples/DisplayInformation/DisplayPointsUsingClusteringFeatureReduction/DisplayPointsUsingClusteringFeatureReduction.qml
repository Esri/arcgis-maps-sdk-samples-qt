// [WriteFile Name=DisplayPointsUsingClusteringFeatureReduction, Category=DisplayInformation]
// [Legal]
// Copyright 2023 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            item: PortalItem { itemId: "8916d50c44c746c1aafae001552bad23" }

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                busyIndicator.running = false;
            }
        }

        onMouseClicked: (mouseClick) => {
                            mapView.calloutData.visible = false;
                            if (!busyIndicator.running) {
                                busyIndicator.running = true;
                                const clickTolerance = 10;
                                const returnPopups = true;
                                mapView.identifyLayer(map.operationalLayers.get(0), mouseClick.x, mouseClick.y, clickTolerance, returnPopups);
                            }
                        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus !== Enums.TaskStatusCompleted)
                return;

            busyIndicator.running = false;

            if (!mapView.identifyLayerResult)
                return;

            if (mapView.identifyLayerResult.error) {
                console.log("An error occured:", mapView.identifyLayerResult.error.message, mapView.identifyLayerResult.error.additionalMessage);
                return;
            }

            if (!mapView.identifyLayerResult.popups[0])
                return;

            const popup = mapView.identifyLayerResult.popups[0];
            const popupManager = ArcGISRuntimeEnvironment.createObject("PopupManager", {popup: popup});

            mapView.calloutData.visible = true;
            mapView.calloutData.location = popup.geoElement.geometry;
            contentText.text = popupManager.customHtmlDescription;
        }
    }

    Button {
        id: clusterToggleButton
        width: 250
        height: 50
        text: qsTr("Toggle feature clustering")
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        enabled: !busyIndicator.running
        onClicked: {
            const isEnabled = map.operationalLayers.get(0).featureReduction.enabled;
            map.operationalLayers.get(0).featureReduction.enabled = !isEnabled;
            mapView.calloutData.visible = false;
        }
    }

    Callout {
        id: callout
        calloutData: mapView.calloutData
        implicitWidth: 150
        implicitHeight: contentText.implicitHeight + (contentText.implicitHeight * .05)
        contentItem: Label {
            id: contentText
            text: ""
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: true
    }
}
