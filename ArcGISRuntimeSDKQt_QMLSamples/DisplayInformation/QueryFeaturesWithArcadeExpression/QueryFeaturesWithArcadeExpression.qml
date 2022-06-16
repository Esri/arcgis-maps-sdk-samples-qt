// [WriteFile Name=QueryFeaturesWithArcadeExpression, Category=DisplayInformation]
// [Legal]
// Copyright 2022 Esri.

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
import Esri.ArcGISRuntime 100.15
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Point calloutLocation
    property real xCoor
    property real yCoor

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        PortalItem {
            id: mapPortalItem
            portal: portal
            itemId: "14562fced3474190b52d315bc19127f6"
            //                onErrorChanged: {
            //                    console.log("Error in portal item: %1(%2)".arg(error.message).arg(error.additionalMessage));
            //                }
            onLoadStatusChanged: {
                                    if (loadStatus === Enums.LoadStatusLoaded) {
                //                        map.operationalLayers.forEach(fl => {
                //                                                          if (fl.name !== "RPD Beats  - City_Beats_Border_1128-4500") {
                //                                                              fl.visible = false;
                //                                                          }
                //                                                      });
//                                        map.operationalLayers.append(mapPortalItem);
//                                            let count = map.operationalLayers.count;
//                                            for (let i = 0; i < map.operationalLayers.count; ++i) {
//                                                let currentFeatureLayer = map.operationalLayers.get(i);
//                                                let featureLayerName = currentFeatureLayer.name;
//                                            }
                                    }
            }
        }

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic
            item: mapPortalItem
        }
        //! [show callout qml api snippet]
//         initialize Callout
        calloutData {
//            imageUrl: "qrc:/Samples/DisplayInformation/ShowCallout/RedShinyPin.png"
            title: "Location"
            location: calloutLocation
            detail: "x: " + xCoor + " y: " + yCoor
        }

        Callout {
            id: callout
            calloutData: parent.calloutData
            leaderPosition: leaderPositionEnum.Automatic
        }
//        ! [show callout qml api snippet]

//         display callout on mouseClicked
        onMouseClicked: {
            if (callout.calloutVisible)
                callout.dismiss()
            else
            {
                calloutLocation = mouse.mapPoint;
                xCoor = mouse.mapPoint.x.toFixed(2);
                yCoor = mouse.mapPoint.y.toFixed(2);
                callout.accessoryButtonHidden = true;
                callout.showCallout();
            }
        }

    }
}
