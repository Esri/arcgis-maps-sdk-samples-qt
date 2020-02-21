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
import Esri.ArcGISRuntime 100.8
import Esri.ArcGISRuntime.Toolkit.Controls 100.8 // needed to use Callout in QML

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property Point clickedPoint: null

    MapView {
        id: mapView
        anchors.fill: parent

        onMouseClicked: {
            clickedPoint = mouse.mapPoint;
            mapView.identifyGraphicsOverlayWithMaxResults(graphicsOverlay, mouse.x, mouse.y, 5, false, 1);
        }

        onIdentifyGraphicsOverlayStatusChanged: {
            if (identifyGraphicsOverlayStatus === Enums.TaskStatusCompleted)
            {
                if (locatorTask.geocodeStatus !== Enums.TaskStatusInProgress)
                {
                    locatorTask.reverseGeocodeWithParameters(clickedPoint, reverseGeocodeParameters);
                    console.log("geocoded");
                }
            }
        }

        Map {
            BasemapImageryWithLabels {}
        }

        GraphicsOverlay {
            id: graphicsOverlay

            SimpleRenderer {
                id: simpleRenderer
                PictureMarkerSymbol {
                    id: pinSymbol
                    url: "file:///C:/Users/swa10836/ArcGIS/Runtime/Data/symbol/pin.png"
                }
            }
            Graphic {
                id: graphic
                geometry: clickedPoint
            }
        }

        LocatorTask {
            id: locatorTask
            ReverseGeocodeParameters {
                id: reverseGeocodeParameters
                maxResults: 1
                maxDistance: 1000
            }

            onGeocodeStatusChanged: {
                if (geocodeStatus === Enums.TaskStatusCompleted)
                {
                    if (geocodeResults.length > 0)
                        console.log("found results");
                    if (geocodeResults.length == 0)
                        console.log("no results");
                    console.log("task completed");
                } else {
                    console.log("task not completed");
                }
            }
        }

        onErrorChanged: {console.log(error.message); }
    }

    //    MouseArea {
    //        anchors.fill: parent
    //        onClicked: {
    //            var clickedPoint = mapView.screenToLocation(mouseX, mouseY);
    //            graphic.geometry = clickedPoint;
    //            mapView.identifyGraphicsOverlayWithMaxResults(graphicsOverlay, mouse.x, mouse.y, 5, false, 1);

    //            if (locatorTask.geocodeStatus !== Enums.TaskStatusInProgress)
    //            {
    //                console.log("here");
    //                locatorTask.reverseGeocodeWithParameters(mouse.mapPoint, reverseGeocodeParameters);
    //                console.log(mouse.mapPoint);
    //            }


    //        }
    //    }

}

//console.log(reverseGeocodeParameters);
//if (locatorTask.geocodeStatus === Enums.TaskStatusInProgress)
//{
//    console.log("In progress");
//    //                locatorTask.reverseGeocodeWithParameters(clickedPoint, reverseGeocodeParameters);
//} else if (locatorTask.geocodeStatus === Enums.TaskStatusReady)
//{
//    console.log("ready");
//    locatorTask.reverseGeocodeWithParameters(clickedPoint, reverseGeocodeParameters);
//    console.log("done");
//} else if (locatorTask.geocodeStatus === Enums.TaskStatusErrored)
//{
//    console.log("errored");
//} else if (locatorTask.geocodeStatus === Enums.TaskStatusCompleted)
//{
//    console.log("Completed");
//}
