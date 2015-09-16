// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import Esri.ArcGISRuntime 100.00
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string displayText: "No features selected. Click/Tap to select features."


    // Map view UI presentation at top
    MapView {
        id: mapView
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: messageBar.top
        }

        Map {
            id: map
			BasemapStreets {}

            FeatureLayer {
                id: featureLayer

                selectionColor: "cyan"
                selectionWidth: 3 * scaleFactor

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/DamageAssessment/FeatureServer/0"
                }

                onSelectFeaturesStatusChanged: {
                    if (selectFeaturesStatus === Enums.TaskStatusCompleted) {
                        var count = 0;
                        while (selectFeaturesResult.iterator.hasNext) {
                            selectFeaturesResult.iterator.next();
                            ++count;
                        }

                        displayText = "%1 %2 selected.".arg(count).arg(count > 1 ? "features" : "feature");
                    }
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    mapView.setViewpoint(viewPoint);
                }
            }
        }

        // initial viewPoint
        ViewpointExtent {
            id: viewPoint
            extent: Envelope {
                xMin: -1131596.019761
                yMin: 3893114.069099
                xMax: 3926705.982140
                yMax: 7977912.461790
                spatialReference: SpatialReference {
                    wkid: 102100
                }
            }
        }

        QueryParameters {
            id: params
            outFields: ["*"]
        }

        onMouseClicked: {
            // create an envelope with some tolerance and query for feature selection within that envelope
            var tolerance = 22 * scaleFactor;
            var mapTolerance = tolerance * unitsPerPixel;
            var envJson = {"xmin" : mouse.mapX - mapTolerance, "ymin" : mouse.mapY - mapTolerance, "xmax" : mouse.mapX + mapTolerance, "ymax" : mouse.mapY + mapTolerance,
                "spatialReference" : {"wkid": 102100}};
            var envelope = ArcGISRuntimeEnvironment.createObject("Envelope", {"json" : envJson});

            // set the envelope as the geometry for the query parameter
            params.geometry = envelope;
            // query and select the features
            featureLayer.selectFeaturesWithQuery(params, Enums.SelectionModeNew);
        }
    }

    Rectangle {
        id: messageBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 30 * scaleFactor
        color: "lightgrey"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }

        Text {
            id: msgText
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10 * scaleFactor
            }
            text: displayText
            font.pixelSize: 14 * scaleFactor
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
