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

import QtQuick
import Esri.ArcGISRuntime
import Esri.ArcGISExtras
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Point mapPoint: null

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
        }

        Map {
            id: map
            item: mapPortalItem

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {

                    // Set the visibility of all the other layers to false to avoid cluttering the UI
                    mapView.map.operationalLayers.get(1).visible = false;
                    mapView.map.operationalLayers.get(2).visible = false;
                    mapView.map.operationalLayers.get(3).visible = false;
                }
            }
        }
        onMouseClicked: mouse => {
            mapPoint = screenToLocation(mouse.x, mouse.y);
            if (identifyLayerStatus !== Enums.TaskStatusInProgress) {
                if (map.operationalLayers.get(0))
                {
                    identifyLayer(map.operationalLayers.get(0), mouse.x, mouse.y, 12, false, 1);
                }
            }
        }
        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                if (identifyLayerResult.geoElements.length < 1) {
                    callout.dismiss();
                    return;
                }
                else if (identifyLayerResult.geoElements.length > 0) {
                    const geoElement = identifyLayerResult.geoElements[0];

                    showEvaluatedArcadeInCallout(geoElement, mapPoint);
                }
                callout.calloutData.title = "RPD Beats"
            }
        }

        Callout {
            id: callout
            calloutData: parent.calloutData
            accessoryButtonHidden: true
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    function showEvaluatedArcadeInCallout(feature, mapPoint) {
        const expressionString = "var crimes = FeatureSetByName($map, \'Crime in the last 60 days\');\n
        return Count(Intersects($feature, crimes));";
        const arcadeExpression = ArcGISRuntimeEnvironment.createObject("ArcadeExpression", {expression: expressionString});
        const arcadeEvaluator = ArcGISRuntimeEnvironment.createObject("ArcadeEvaluator", {initExpression: arcadeExpression, initProfile: Enums.ArcadeProfileFormCalculation});

        const profileVariables = {
            "$feature": feature,
            "$map": map
        };
        arcadeEvaluator.evaluateStatusChanged.connect(()=> {
                                                          if (arcadeEvaluator.evaluateStatus !== Enums.TaskStatusCompleted)
                                                          return;

                                                          const result = arcadeEvaluator.evaluateResult.result;
                                                          callout.calloutData.detail = "Crimes in the last 60 days: " + result;
                                                          callout.calloutData.location = mapPoint;
                                                          callout.showCallout();
                                                      });
        const evaluationResult = arcadeEvaluator.evaluate(profileVariables);
    }
}
