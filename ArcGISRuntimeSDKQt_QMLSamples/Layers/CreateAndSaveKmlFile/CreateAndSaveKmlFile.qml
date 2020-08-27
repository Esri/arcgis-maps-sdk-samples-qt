// [WriteFile Name=CreateAndSaveKmlFile, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Dialogs
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapDarkGrayCanvasVector {}

            // add a KML Layer
            KmlLayer {
                KmlDataset {
                    id: kmlDataset
                    KmlDocument {
                        id: kmlDocument
                        name: qsTr("KML Sample Document")
                        onSaveStatusChanged: {
                            if (saveStatus === Enums.TaskStatusErrored) {
                                console.log(`Error: ${error.message} - ${error.additionalMessage}`);
                            }
                            else if (saveStatus === Enums.TaskStatusCompleted) {
                                saveCompleteDialog.open();
                            }
                        }
                    }
                }
            }

            // set initial extent
            ViewpointExtent {
                Envelope {
                    id: myViewpoint
                    xMin: -123.0
                    yMin: 33.5
                    xMax: -101.0
                    yMax: 42.0
                    spatialReference: SpatialReference { wkid: 4326 }
                }
            }
        }

    }

    Button {
        anchors{
            left: parent.left
            top: parent.top
            margins: 3
        }
        text: qsTr("Save kmz file")

        onClicked: {
            fileDialog.open();
        }
    }

    PolygonBuilder {
        id: polygonBuilder
        spatialReference: SpatialReference { wkid: 4326 }
    }

    PolylineBuilder {
        id: polylineBuilder
        spatialReference: SpatialReference { wkid: 4326 }
    }

    Point {
        id: point
        x: -117.195800
        y: 34.056295
        spatialReference: SpatialReference { wkid: 4326 }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: kmlDocument.saveStatus === Enums.TaskStatusInProgress
    }

    Dialogs.FileDialog {
        id: fileDialog
        defaultSuffix: "kmz"
        fileMode: Dialogs.FileDialog.SaveFile
        nameFilters: ["Kml files (*.kmz *.kml)"]
        onAccepted: {
            // Write the KML document to the chosen path.
            kmlDocument.saveAs(currentFile);
            close();
        }

        onRejected: {
            close();
        }
    }

    Dialog {
        id: saveCompleteDialog
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        Text {
            id:textLabel
            anchors.centerIn: parent
            text: qsTr("Item saved.")
        }
    }

    KmlStyle {
        id: kmlStyleWithPointStyle
        KmlIconStyle {
            KmlIcon {
                url: "https://static.arcgis.com/images/Symbols/Shapes/BlueStarLargeB.png"
            }
            scale: 1
        }
    }

    KmlStyle {
        id: kmlStyleWithLineStyle
        KmlLineStyle {
            color: "red"
            width: 2
        }
    }

    KmlStyle {
        id: kmlStyleWithPolygonStyle
        KmlPolygonStyle {
            color: "yellow"
        }
    }

    Component.onCompleted: {
        createPolygon();
        createPolyline();
        createPoint();
    }

    function createPoint() {
        addToKmlDocument(point, kmlStyleWithPointStyle);
    }

    function createPolygon() {
        polygonBuilder.addPointXY(-109.048, 40.998);
        polygonBuilder.addPointXY(-102.047, 40.998);
        polygonBuilder.addPointXY(-102.037, 36.989);
        polygonBuilder.addPointXY(-109.048, 36.998);
        addToKmlDocument(polygonBuilder.geometry, kmlStyleWithPolygonStyle);
    }

    function createPolyline() {
        polylineBuilder.addPointXY(-119.992, 41.989);
        polylineBuilder.addPointXY(-119.994, 38.994);
        polylineBuilder.addPointXY(-114.620, 35.0);
        addToKmlDocument(polylineBuilder.geometry, kmlStyleWithLineStyle);
    }

    function addToKmlDocument(geometry, kmlStyle) {
        const kmlGeometry = ArcGISRuntimeEnvironment.createObject("KmlGeometry", {
                                                                      geometry: geometry,
                                                                      altitudeMode: Enums.KmlAltitudeModeClampToGround
                                                                  });
        const kmlPlacemark = ArcGISRuntimeEnvironment.createObject("KmlPlacemark");
        kmlPlacemark.geometriesListModel.append(kmlGeometry);
        kmlPlacemark.style = kmlStyle;
        kmlDocument.childNodesListModel.append(kmlPlacemark);
    }
}
