// [WriteFile Name=FeatureLayer_Selection, Category=Features]
// [Legal]
// Copyright 2016 Esri.

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
    width: 800
    height: 600

    
    property string displayText: "Click or tap to select features."


    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: messageBar.top
        }
        wrapAroundMode: Enums.WrapAroundModeDisabled

        Map {
            id: map
            BasemapLightGrayCanvas {}

            FeatureLayer {
                id: featureLayer

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "https://services1.arcgis.com/4yjifSiIG17X0gW4/arcgis/rest/services/GDP_per_capita_1960_2016/FeatureServer/0"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(viewPoint);
                }
            }
        }

        // initial viewpoint
        ViewpointExtent {
            id: viewPoint
            Envelope {
                xMin: -6603299.491810
                yMin: 1679677.742046
                xMax: 9002253.947487
                yMax: 8691318.054732
                SpatialReference {
                    wkid: 3857
                }
            }
        }

        //! [identify feature layer qml api snippet]
        onMouseClicked: {
            const tolerance = 22;
            const returnPopupsOnly = false;
            const maximumResults = 1000;
            mapView.identifyLayerWithMaxResults(featureLayer, mouse.x, mouse.y, tolerance, returnPopupsOnly, maximumResults);
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                // clear any previous selections
                featureLayer.clearSelection();

                // create an array to store the features
                const identifiedObjects = [];
                for (let i = 0; i < identifyLayerResult.geoElements.length; i++){
                    const elem = identifyLayerResult.geoElements[i];
                    identifiedObjects.push(elem);
                }
                // cache the number of identifyLayerResult
                const count = identifyLayerResult.geoElements.length;

                // select the features in the feature layer
                featureLayer.selectFeatures(identifiedObjects);
                displayText = "%1 %2 selected.".arg(count).arg(count > 1 ? "features" : "feature");
            }
        }
        //! [identify feature layer qml api snippet]
    }

    Rectangle {
        id: messageBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 30
        color: "lightgrey"
        border {
            width: 0.5
            color: "black"
        }

        Text {
            id: msgText
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
            }
            text: displayText
            font.pixelSize: 14
        }
    }
}
