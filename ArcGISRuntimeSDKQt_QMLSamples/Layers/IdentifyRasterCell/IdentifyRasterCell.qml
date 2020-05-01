// [WriteFile Name=IdentifyRasterCell, Category=Layers]
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
import Esri.ArcGISRuntime 100.8
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Controls 100.8 // needed to use Callout in QML

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster-file/"
    property Point clickedPoint: null
    property string calloutText: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Callout {
            id: callout
            calloutData: parent.calloutData
            autoAdjustWidth: false
            calloutWidth: 300
            accessoryButtonHidden: true
            calloutContent: customComponent
        }

        Component {
              id: customComponent

              Text {
                  id: componentText
                  text: calloutText
                  Binding {
                      target: callout
                      value: contentHeight + callout.calloutFramePadding
                      property: "calloutHeight"
                  }

                  wrapMode: Text.WordWrap
              }
          }

        Map {
            BasemapImagery {}
            RasterLayer {
                id: rasterLayer
                Raster {
                    path: dataPath + "Shasta.tif"
                }

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    mapView.setViewpointGeometry(fullExtent);
                }
            }
        }

        onMouseClicked: {
            clickedPoint = screenToLocation(mouse.x, mouse.y);
            if (identifyLayerStatus !== Enums.TaskStatusInProgress) {
                identifyLayer(rasterLayer, mouse.x, mouse.y, 10, false, 1);
            }
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus !== Enums.TaskStatusCompleted)
                return;

            for (let i = 0; i < identifyLayerResult.geoElements.length; i++) {
                calloutText = "";
                const attributes = identifyLayerResult.geoElements[i].attributes;
                const attributeNames = attributes.attributeNames;

                for (let j = 0; j < attributeNames.length; j++) {
                    calloutText = calloutText + attributeNames[j] + ": " + attributes.attributeValue(attributeNames[j]) + "\n";
                }

                const xPoint = identifyLayerResult.geoElements[i].geometry.extent.xMin;
                const yPoint = identifyLayerResult.geoElements[i].geometry.extent.yMin;

                calloutText = calloutText + "X: " + xPoint.toFixed(2) + " Y: " + yPoint.toFixed(2);

                callout.calloutData.location = clickedPoint;
                callout.showCallout();
            }
        }
    }
}
