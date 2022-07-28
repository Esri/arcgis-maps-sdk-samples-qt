// [WriteFile Name=ExportVectorTiles, Category=Layers]
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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // Create a path to store the vector tile package
    readonly property url vectorTileCachePath: System.temporaryFolder.url + "/vectorTiles_%1.vtpk".arg(new Date().getTime().toString())
    // Create a path to store the styling resources (in this case, the night mode version of the layer)
    readonly property url itemResourcePath: System.temporaryFolder.url + "/itemResources_%1".arg(new Date().getTime().toString())
    property int exportProgress: 0

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            initBasemapStyle: Enums.BasemapStyleArcGISStreetsNight
            initialViewpoint: ViewpointCenter {
                id: viewpoint
                Point {
                    x: -117.181
                    y: 34.049
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 1e4
            }
        }



        GraphicsOverlay {
            id: graphicsOverlay

            Graphic {
                id: exportAreaGraphic
                symbol: SimpleLineSymbol {
                    style: Enums.SimpleLineSymbolStyleSolid
                    color: "green"
                    width: 3
                }
            }
        }

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        id: exportProgressWindow
        anchors.centerIn: parent

        color: "white"
        visible: false

        border {
            color: "black"
            width: 2
        }

        Column {
            id: exportProgressColumn
            anchors.centerIn: parent
            spacing: 10

            BusyIndicator {
                id: busyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: statusText
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Export in progress..."
                font.pixelSize: 16
            }

            Text {
                id: progressText
                anchors.horizontalCenter: parent.horizontalCenter
                text: exportProgress + "% Completed"
                font.pixelSize: 16
            }

            onWidthChanged: {
                exportProgressWindow.width = exportProgressColumn.width + 20
            }

            onHeightChanged: {
                exportProgressWindow.height = exportProgressColumn.height + 20
            }
        }
    }

    // Create an extent rectangle for selecting the offline area
    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent

        width: parent.width - parent.width * 0.1
        height: parent.height - parent.height * 0.25
        color: "transparent"
        border {
            color: "red"
            width: 3
        }
    }

    Button {
        id: button
        width: buttonImage.width + buttonText.width + (buttonTextRow.spacing * 2)

        Row {
            id: buttonTextRow
            spacing: 5
            Image {
                id: buttonImage
                width: button.height
                height: button.height
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/Samples/Layers/ExportVectorTiles/download.png"
            }

            Text {
                id: buttonText
                text: "Export area"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        anchors {
            bottom: parent.bottom
            bottomMargin: parent.height * .05
            horizontalCenter: parent.horizontalCenter
        }

        onClicked: {
            startExport(extentRectangle.x, (extentRectangle.y + extentRectangle.height), (extentRectangle.x + extentRectangle.width), extentRectangle.y);
            extentRectangle.visible = false;
            button.visible = false;
            exportProgressWindow.visible = true;
            statusText.text = "Exporting vector tiles";
        }
    }

    function startExport(xSW, ySW, xNE, yNE) {
        const vectorTiledLayer = map.basemap.baseLayers.get(0);
        if (vectorTiledLayer.layerType !== Enums.LayerTypeArcGISVectorTiledLayer)
            return;

        // Create an envelope from the QML rectangle corners
        const pointSW = mapView.screenToLocation(xSW, ySW);
        const pointNE = mapView.screenToLocation(xNE, yNE);
        const extent = ArcGISRuntimeEnvironment.createObject("Envelope", {
            xMin: pointSW.x,
            xMax: pointNE.x,
            yMin: pointNE.y,
            yMax: pointSW.y
        });
        const exportArea = GeometryEngine.project(extent, vectorTiledLayer.spatialReference);
        extentRectangle.visible = false;
        exportAreaGraphic.geometry = exportArea;

        exportVectorTilesTask.url = vectorTiledLayer.url;

        // Instantiate export parameters to create the export job with
        exportVectorTilesTask.createDefaultExportVectorTilesParameters(exportArea, mapView.mapScale * 0.1);
    }

    ExportVectorTilesTask {
        id: exportVectorTilesTask

        onCreateDefaultExportVectorTilesParametersStatusChanged: {
            if (createDefaultExportVectorTilesParametersStatus !== Enums.TaskStatusCompleted)
                return;

            // Using the reduced fonts service will reduce the download size of a vtpk by around 80 Mb
            // It is useful for taking the basemap offline but not recommended if you plan to later upload the vtpk
            defaultExportVectorTilesParameters.esriVectorTilesDownloadOption = Enums.EsriVectorTilesDownloadOptionUseReducedFontsService;

            const exportVectorTilesJob = exportVectorTilesWithStyleResources(defaultExportVectorTilesParameters, vectorTileCachePath, itemResourcePath);

            exportVectorTilesJob.resultChanged.connect(() => {
                if (exportVectorTilesJob.result !== null) {
                    // Create a vector tiled layer when the download is completed
                    const exportedVectorTiledLayer = ArcGISRuntimeEnvironment.createObject("ArcGISVectorTiledLayer", {
                        vectorTileCache: exportVectorTilesJob.result.vectorTileCache,
                        itemResourceCache: exportVectorTilesJob.result.itemResourceCache
                    });
                    const basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
                    basemap.baseLayers.append(exportedVectorTiledLayer);
                    map.basemap = basemap;
                    exportProgressWindow.visible = false;
                }
            });

            // Display the download progress to the user
            exportVectorTilesJob.progressChanged.connect(() => {
                                                             exportProgress = exportVectorTilesJob.progress;
                                                         });

            // Start the export job once export parameters have been created
            exportVectorTilesJob.start();
        }
    }
}

