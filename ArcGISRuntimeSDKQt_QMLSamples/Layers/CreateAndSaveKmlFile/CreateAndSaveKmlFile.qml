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
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import Esri.ArcGISRuntime 100.6

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool styling: false
    property var kmlLayer

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
        }

        Button {
            anchors{
                left: parent.left
                top: parent.top
                margins: 3
            }
            text: qsTr("Save kmz file")

            onClicked: {
                fileDialog.visible = true;
            }
        }

        GraphicsOverlay {
            id: graphicsOverlay
        }
    }

    KmlDocument {
        id: kmlDocument
        name: qsTr("KML Sample Document")
        onSaveStatusChanged: {
            if (saveStatus === Enums.TaskStatusErrored) {
                console.log("Error: %1 - %2".arg(error.message).arg(error.additionalMessage))
            }
        }
    }

    Graphic {
        id: polygonGraphic
    }

    Graphic {
        id: polylineGraphic
    }

    Graphic {
        id: pointGraphic
    }

    SimpleFillSymbol {
        id: polygonSymbol
        style: Enums.SimpleFillSymbolStyleSolid
        color: "red"
    }

    SimpleLineSymbol {
        id: polylineSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "blue"
        width: 5
        antiAlias: true
    }

    SimpleMarkerSymbol {
        id: pointSymbol
        style: Enums.SimpleMarkerSymbolStyleDiamond
        color: "green"
        size: 10
    }

    PolygonBuilder {
        id: polygonBuilder
        spatialReference: SpatialReference.createWgs84();
    }

    PolylineBuilder {
        id: polylineBuilder
        spatialReference: SpatialReference.createWgs84();
    }

    Point {
        id: point
        x: -117.195800
        y: 34.056295
        spatialReference: SpatialReference.createWgs84();
    }

    Envelope {
        id: myViewpoint
        xMin: -123.0
        yMin: 33.5
        xMax: -101.0
        yMax: 42.0
        spatialReference: SpatialReference.createWgs84();
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: kmlDocument.saveStatus === Enums.TaskStatusInProgress
    }

    FileDialog {
        id: fileDialog
        defaultSuffix: "kmz"
        selectExisting: false
        onAccepted: {
            kmlDocument.saveAs(fileUrl);
            visible: false;
        }

        onRejected: {
            visible: false;
        }
    }

    Component.onCompleted: {
        createPolygon();
        createPolyline();
        createPoint();
        mapView.setViewpointGeometryAndPadding(myViewpoint, 10);
    }

    function createPolygon() {
        polygonBuilder.addPointXY(-109.048, 40.998);
        polygonBuilder.addPointXY(-102.047, 40.998);
        polygonBuilder.addPointXY(-102.037, 36.989);
        polygonBuilder.addPointXY(-109.048, 36.998);
        polygonGraphic.geometry = polygonBuilder.geometry;
        polygonGraphic.symbol = polygonSymbol;
        graphicsOverlay.graphics.append(polygonGraphic);

        addToKmlDocument(polygonBuilder.geometry);
    }

    function createPolyline() {
        polylineBuilder.addPointXY(-119.992, 41.989);
        polylineBuilder.addPointXY(-119.994, 38.994);
        polylineBuilder.addPointXY(-114.620, 35.0);
        polylineGraphic.geometry = polylineBuilder.geometry;
        polylineGraphic.symbol = polylineSymbol;
        graphicsOverlay.graphics.append(polylineGraphic);

        addToKmlDocument(polylineBuilder.geometry);
    }

    function createPoint() {
        pointGraphic.geometry = point;
        pointGraphic.symbol = pointSymbol;
        graphicsOverlay.graphics.append(pointGraphic);

        addToKmlDocument(point);
    }

    function addToKmlDocument(geometry) {
        let kmlGeometry = ArcGISRuntimeEnvironment.createObject("KmlGeometry", {
                                                                geometry: geometry,
                                                                altitudeMode: Enums.KmlAltitudeModeClampToGround
                                                            });
        let kmlPlacemark = ArcGISRuntimeEnvironment.createObject("KmlPlacemark");
        kmlPlacemark.geometriesListModel.append(kmlGeometry);
        kmlDocument.childNodesListModel.append(kmlPlacemark);
    }
}
