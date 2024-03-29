// [WriteFile Name=IdentifyKmlFeatures, Category=Layers]
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

import QtQuick
import QtQuick.Controls
import Esri.ArcGISRuntime
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Point clickedPoint: null
    property string calloutText: ""
    readonly property url layerUrl: "https://www.wpc.ncep.noaa.gov/kml/noaa_chart/WPC_Day1_SigWx_latest.kml"

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            id: callout
            calloutData: parent.calloutData
            implicitWidth: 150
            implicitHeight: contentText.implicitHeight + (contentText.implicitHeight * .05)
            contentItem: Label {
                id: contentText
                text: calloutText
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISDarkGray
            }

            // start zoomed in over the US
            ViewpointExtent {
                Envelope {
                    xMin: -19195297.778679
                    yMin: 512343.939994
                    xMax: -3620418.579987
                    yMax: 8658913.035426
                    SpatialReference {
                        wkid: 3857
                    }
                }
            }

            KmlLayer {
                id: forecastLayer
                KmlDataset {
                    url: layerUrl
                }
            }
        }

        onMouseClicked: mouse => {
            clickedPoint = screenToLocation(mouse.x, mouse.y);
            if (identifyLayerStatus !== Enums.TaskStatusInProgress) {
                identifyLayerWithMaxResults(forecastLayer, mouse.x, mouse.y, 15, false, 1);
            }
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                if (identifyLayerResult.geoElements.length < 1) {
                    callout.dismiss();
                    return;
                }
                // Google Earth only displays the placemarks with description or extended data. To
                // match its behavior, add a description placeholder if the data source is empty
                if (!identifyLayerResult.geoElements[0].description)
                    identifyLayerResult.geoElements[0].description = "Weather condition"

                calloutText = identifyLayerResult.geoElements[0].balloonContent;
                callout.calloutData.location = clickedPoint;
                callout.showCallout();
            }
        }
    }
}
