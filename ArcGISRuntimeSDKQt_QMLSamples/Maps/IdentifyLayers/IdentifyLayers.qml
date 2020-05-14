// [WriteFile Name=IdentifyLayers, Category=Maps]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property string msgText: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            // add a map image layer and hide 2 of the sublayers
            ArcGISMapImageLayer {
                url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/SampleWorldCities/MapServer"
                onLoadStatusChanged: {
                    if (error) {
                        console.log("error:", error.message, error.additionalMessage)
                        return;
                    }

                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    mapImageSublayers.get(1).visible = false;
                    mapImageSublayers.get(2).visible = false;
                }
            }

            // add a feature layer
            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"
                }
            }

            // set an initial viewpoint
            ViewpointCenter {
                targetScale: 68015210

                Point {
                    x: -10977012.785807
                    y: 4514257.550369
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }

        // identify layers on mouse click
        onMouseClicked: {
            const screenX = mouse.x;
            const screenY = mouse.y;
            const tolerance = 12;
            const returnPopups = false;
            const maxResults = 10;
            mapView.identifyLayersWithMaxResults(screenX, screenY, tolerance, returnPopups, maxResults);
        }

        // handle the identify results
        onIdentifyLayersStatusChanged: {
            if (identifyLayersStatus !== Enums.TaskStatusCompleted)
                return;

            // reset the message string
            msgText = "";

            // loop through the results
            const results = mapView.identifyLayersResults;
            for (let i = 0; i < results.length; i++) {
                const result = results[i];
                const count = geoElementsCountFromResult(result);
                const layerName = result.layerContent.name;
                msgText += "%1 : %2".arg(layerName).arg(count);
                // add new line character if not the final element in array
                if (i !== results.length)
                    msgText += "\n";
            }

            // show the message box
            if (msgText.length > 0)
                msgDialog.open();
        }

        // handle any errors on the map view
        onErrorChanged: {
            if (error)
                console.log("error:", error.message, error.additionalMessage)
        }
    }

    function geoElementsCountFromResult(identifyLayerResult) {
        // create temp array
        const tempResults = [identifyLayerResult];

        // use Depth First Search approach to handle recursion
        let count = 0;
        let index = 0;

        while (index < tempResults.length) {
            //get the result object from the array
            const identifyResult = tempResults[index];

            // update count with geoElements from the result
            count += identifyResult.geoElements.length;

            // check if the result has any sublayer results
            // if yes then add those result objects in the tempResults
            // array after the current result
            if (identifyResult.sublayerResults.length > 0) {
                tempResults.push(identifyResult.sublayerResults[index]);
            }

            // update the count and repeat
            index += 1;
        }

        return count;
    }

    Dialog {
        id: msgDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        property alias text : textLabel.text
        Text {
            id: textLabel
            text: msgText
        }
    }
}
