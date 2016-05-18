// [WriteFile Name=ExportTiles, Category=Layers]
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property Envelope tileCacheExtent: null
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"
    property string statusText: ""
    property string tiledServiceUrl: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer"

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        Map {
            id: map
            // Nest an ArcGISTiledLayer in the Basemap
            Basemap {
                ArcGISTiledLayer {
                    id: tiledLayer
                    url: tiledServiceUrl
                }
            }
        }
    }

    // Create ExportTileCacheTask
    ExportTileCacheTask {
        id: exportTask
        mapServiceInfo: tiledLayer.mapServiceInfo

        function generateDefaultParameters() {
            // generate the default parameters with the extent and map scales specified
            var params = exportTask.createDefaultExportTileCacheParameters(tileCacheExtent, mapView.mapScale, tiledLayer.maxScale);

            // export the cache with the parameters
            executeExportTileCacheTask(params);
        }

        function executeExportTileCacheTask(params) {
            // execute the asynchronous task and obtain the job
            var exportJob = exportTask.exportTileCacheWithParameters(params, dataPath + "outputTileCache.tpk");

            // show the export window
            exportWindow.visible = true;

            // connect to the job's status changed signal to know once it is done
            exportJob.jobStatusChanged.connect(function() {
                switch(exportJob.jobStatus) {
                case Enums.JobStatusFailed:
                    statusText = "Export failed";
                    exportWindow.hideWindow(5000);
                    break;
                case Enums.JobStatusNotStarted:
                    statusText = "Job not started";
                    break;
                case Enums.JobStatusPaused:
                    statusText = "Job paused";
                    break;
                case Enums.JobStatusStarted:
                    statusText = "In progress...";
                    break;
                case Enums.JobStatusSucceeded:
                    statusText = "Complete";
                    exportWindow.hideWindow(1500);
                    displayOutputTileCache(exportJob.result);
                    break;
                default:
                    break;
                }
            });

            exportJob.start();
        }

        function displayOutputTileCache(tileCache) {
            // create a new tiled layer from the output tile cache
            var tiledLayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer", { tileCache: tileCache } );

            // create a new basemap with the tiled layer
            var basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
            basemap.baseLayers.append(tiledLayer);

            // set the new basemap on the map
            map.basemap = basemap;

            // zoom to the new layer and hide window once loaded
            tiledLayer.loadStatusChanged.connect(function() {
                if (tiledLayer.loadStatus === Enums.LoadStatusLoaded) {
                    extentRectangle.visible = false;
                    downloadButton.visible = false;
                    mapView.setViewpointScale(mapView.mapScale * .5);
                }
            });
        }
    }

    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - (50 * scaleFactor)
        height: parent.height - (125 * scaleFactor)
        color: "transparent"
        border {
            color: "red"
            width: 3 * scaleFactor
        }
    }

    // Create the download button to export the tile cache
    Rectangle {
        id: downloadButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 10 * scaleFactor
        }

        width: 130 * scaleFactor
        height: 35 * scaleFactor
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 5
        border {
            color: "#585858"
            width: 1 * scaleFactor
        }

        Row {
            anchors.fill: parent
            spacing: 5
            Image {
                width: 38 * scaleFactor
                height: width
                source: "qrc:/Samples/Layers/ExportTiles/download.png"
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Export tiles"
                font.pixelSize: 14 * scaleFactor
                color: "#474747"
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: downloadButton.pressed = true
            onReleased: downloadButton.pressed = false
            onClicked: {
                getRectangleEnvelope();
            }

            function getRectangleEnvelope() {
                var corner1 = mapView.screenToLocation(extentRectangle.x, extentRectangle.y);
                var corner2 = mapView.screenToLocation((extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
                var envBuilder = ArcGISRuntimeEnvironment.createObject("EnvelopeBuilder");
                envBuilder.setCorners(corner1, corner2);
                tileCacheExtent = GeometryEngine.project(envBuilder.geometry, SpatialReference.createWebMercator());
                exportTask.generateDefaultParameters();
            }
        }
    }

    // Create a window to display the export window
    Rectangle {
        id: exportWindow
        anchors.fill: parent
        color: "transparent"
        visible: false
        clip: true

        GaussianBlur {
            anchors.fill: exportWindow
            source: mapView
            radius: 40
            samples: 20
            rotation: 180
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 125 * scaleFactor
            height: 100 * scaleFactor
            color: "lightgrey"
            opacity: 0.8
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1 * scaleFactor
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                spacing: 10

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: statusText
                    font.pixelSize: 16 * scaleFactor
                }
            }
        }

        Timer {
            id: hideWindowTimer

            onTriggered: exportWindow.visible = false;
        }

        function hideWindow(time) {
            hideWindowTimer.interval = time;
            hideWindowTimer.restart();
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }

    FileFolder {
        path: dataPath

        // create the data path if it does not yet exist
        Component.onCompleted: {
            if (!exists) {
                makePath(dataPath);
            }
        }
    }
}
