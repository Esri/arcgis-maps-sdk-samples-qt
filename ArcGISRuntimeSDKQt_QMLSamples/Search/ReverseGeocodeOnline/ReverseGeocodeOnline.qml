// [WriteFile Name=ReverseGeocodeOnline, Category=Search]
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
import QtQuick.Controls 2.6
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.8
import Esri.ArcGISRuntime.Toolkit.Controls 100.8 // needed to use Callout in QML

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/symbol"
    property Point clickedPoint: null

    MapView {
        id: mapView
        anchors.fill: parent

        Callout {
            id: callout
            calloutData: parent.calloutData
            accessoryButtonHidden: true
            leaderPosition: leaderPositionEnum.Top
        }
        onMouseClicked: {
            clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
            mapView.identifyGraphicsOverlayWithMaxResults(graphicsOverlay, mouse.x, mouse.y, 5, false, 1);
        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus === Enums.TaskStatusCompleted)
            {
                if (locatorTask.geocodeStatus !== Enums.TaskStatusInProgress)
                {
                    locatorTask.reverseGeocodeWithParameters(clickedPoint, reverseGeocodeParameters);
                }
            }
        }

        Map {
            BasemapImageryWithLabels {}

            ViewpointCenter {
                Point {
                    x: -13042254.715252
                    y: 3857970.236806
                    SpatialReference {
                        wkid: 3857
                    }
                }
                targetScale: 3e4
            }
        }

        GraphicsOverlay {
            id: graphicsOverlay

            SimpleRenderer {
                PictureMarkerSymbol {
                    url: dataPath + "/pin.png"
                    height: 72
                    width: 19
                }
            }

            Graphic {
                geometry: clickedPoint
            }
        }

        ReverseGeocodeParameters {
            id: reverseGeocodeParameters
            outputSpatialReference: mapView.spatialReference
        }

        LocatorTask {
            id: locatorTask
            url: "http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer"

            onGeocodeStatusChanged: {
                if (geocodeStatus === Enums.TaskStatusCompleted)
                {
                    if (geocodeResults.length > 0)
                    {
                        var address = geocodeResults[0].label;
                        mapView.setViewpointCenter(geocodeResults[0].displayLocation);
                        mapView.calloutData.location = clickedPoint;
                        mapView.calloutData.title = address.split(",")[0];
                        mapView.calloutData.detail = address.substring(address.indexOf(", ") + 2);
                        callout.showCallout();
                    }
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        visible: true
        running: locatorTask.geocodeStatus === Enums.TaskStatusInProgress
    }
}
