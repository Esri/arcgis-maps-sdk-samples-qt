// [WriteFile Name=EditFeaturesWithFeatureLinkedAnnotation, Category=EditData]
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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime 100.11
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"
    readonly property string pointsFeatureLayerName: "Loudoun_Address_Points_1"
    readonly property string pointsAnnoLayerName: "Loudoun_Address_PointsAnno_1"
    readonly property string linesFeatureLayerName: "ParcelLines_1"
    readonly property string linesAnnoLayerName: "ParcelLinesAnno_1"
    readonly property string ad_address: "AD_ADDRESS"
    readonly property string st_str_nam: "ST_STR_NAM"
    property var selectedFeature: null
    property var pointsFeatureLayer: null
    property var linesFeatureLayer: null
    property string streetNameText: ""
    property string addressText: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapLightGrayCanvasVector {}

            initialViewpoint: viewpoint
        }

        onMouseClicked: {
            const clickPoint = mapView.screenToLocation(mouse.x, mouse.y);

            if (updateWindow.visible)
                updateWindow.visible = false;

            if (selectedFeature) {
                moveFeature(clickPoint);
            } else {
                mapView.identifyLayers(mouse.x, mouse.y, 10, false);
            }
        }

        onIdentifyLayersStatusChanged: {
            if (mapView.identifyLayersStatus !== Enums.TaskStatusCompleted)
                return;

            const results = mapView.identifyLayersResults;

            for (let i = 0; i < results.length; i++) {

                const layerContent = identifyLayersResults[i].layerContent;

                // only select features if the layer content is a Feature Layer
                if (layerContent.name === pointsFeatureLayerName || layerContent.name === linesFeatureLayerName) {

                    selectedFeature = results[i].geoElements[0];
                    const geometry = selectedFeature.geometry;

                    if (geometry.geometryType === Enums.GeometryTypePoint) {

                        // update QML text fields with the attributes for the selected point feature
                        addressText = selectedFeature.attributes.attributeValue(ad_address);
                        streetNameText = selectedFeature.attributes.attributeValue(st_str_nam);
                        pointsFeatureLayer.selectFeature(selectedFeature);
                        updateWindow.visible = true;
                    } else if (geometry.geometryType === Enums.GeometryTypePolyline ) {

                        const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {
                                                                                          geometry: geometry,
                                                                                          spatialReference: Factory.SpatialReference.createWgs84()
                                                                                      });

                        // if the selected feature is a polyline with any part containing more than one segment
                        // (i.e. a curve) do not select it
                        if (polylineBuilder.parts.part(0).segmentCount > 1) {
                            clearSelection();
                            return;
                        }
                        linesFeatureLayer.selectFeature(selectedFeature);
                        return;
                    }
                }
            }
        }
    }

    // Create the initial Viewpoint
    ViewpointCenter {
        id: viewpoint
        // Specify the center Point
        center: Point {
            x: -77.41589
            y: 39.0204
            spatialReference: SpatialReference { wkid: 4326 }
        }
        // Specify the scale
        targetScale: 2257
    }

    Geodatabase {
        id: geodatabase
        path: dataPath + "/geodatabase/loudoun_anno.geodatabase"

        onLoadStatusChanged: {
            if (Enums.LoadStatusLoaded !== geodatabase.loadStatus)
                return;

            // create feature layers from tables in the geodatabase
            const pointFeatureTable = geodatabase.geodatabaseFeatureTablesByTableName[pointsFeatureLayerName];
            pointsFeatureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {
                                                                           featureTable: pointFeatureTable
                                                                       });
            map.operationalLayers.append(pointsFeatureLayer);

            const linesFeatureTable = geodatabase.geodatabaseFeatureTablesByTableName[linesFeatureLayerName];
            linesFeatureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {
                                                                          featureTable: linesFeatureTable
                                                                      });
            map.operationalLayers.append(linesFeatureLayer);

            // create annotation layers from tables in the geodatabase
            const pointAnnoFeatureTable = geodatabase.geodatabaseAnnotationTablesByTableName[pointsAnnoLayerName];
            const pointAnnoFeatureLayer = ArcGISRuntimeEnvironment.createObject("AnnotationLayer", {
                                                                                    featureTable: pointAnnoFeatureTable
                                                                                });
            map.operationalLayers.append(pointAnnoFeatureLayer);

            const linesAnnoFeatureTable = geodatabase.geodatabaseAnnotationTablesByTableName[linesAnnoLayerName];
            const linesAnnoFeatureLayer = ArcGISRuntimeEnvironment.createObject("AnnotationLayer", {
                                                                                    featureTable: linesAnnoFeatureTable
                                                                                });
            map.operationalLayers.append(linesAnnoFeatureLayer);
        }
    }

    QueryParameters {
        id: params
    }

    // Update Window
    Rectangle {
        id: updateWindow
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
        radius: 10
        visible: false

        GaussianBlur {
            anchors.fill: updateWindow
            source: mapView
            radius: 40
            samples: 20
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        GridLayout {
            columns: 2
            anchors.margins: 5

            Text {
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Update Attributes")
                font.pixelSize: 16
            }

            Text {
                text: qsTr("AD_ADDRESS:")
                Layout.margins: 5
            }

            TextField {
                id: attAddressTextField
                selectByMouse: true
                text: addressText
            }

            Text {
                text: qsTr("ST_STR_NAM:")
                Layout.margins: 5
            }

            TextField {
                id: attStreetTextField
                text: streetNameText
                selectByMouse: true
            }

            Row {
                Layout.alignment: Qt.AlignRight
                Layout.columnSpan: 2
                height: childrenRect.height
                spacing: 5

                Button {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignRight
                    text: qsTr("Update")
                    onClicked: {
                        updateWindow.visible = false;

                        if (!selectedFeature)
                            return;

                        // update the two attributes with the inputted text.
                        selectedFeature.attributes.replaceAttribute(ad_address, attAddressTextField.text);
                        selectedFeature.attributes.replaceAttribute(st_str_nam, attStreetTextField.text);
                        selectedFeature.featureTable.updateFeature(selectedFeature);
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    // once the cancel button is clicked, hide the window and reset text fields
                    onClicked: {
                        updateWindow.visible = false;
                        streetNameText = "";
                        addressText = "";
                    }
                }
            }
        }
    }

    function clearSelection() {
        pointsFeatureLayer.clearSelection();
        linesFeatureLayer.clearSelection();

        selectedFeature = null;
    }

    function moveFeature(mapPoint) {
        const geometry = selectedFeature.geometry;
        const workingPoint = GeometryEngine.project(mapPoint, geometry.spatialReference);

        if (geometry.geometryType === Enums.GeometryTypePolyline) {

            // get nearest vertex to the map point on the selected polyline
            const nearestVertex = GeometryEngine.nearestVertex(geometry, workingPoint);

            const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {
                                                                              geometry: geometry,
                                                                              spatialReference: Factory.SpatialReference.createWgs84()
                                                                          });

            // get part of polyline nearest to map point
            const part = polylineBuilder.parts.part(nearestVertex.partIndex);

            // remove nearest point to map point
            part.removePoint(nearestVertex.pointIndex);

            // add the map point as a new point
            part.addPoint(workingPoint);

            selectedFeature.geometry = polylineBuilder.geometry;
        } else if (geometry.geometryType === Enums.GeometryTypePoint) {

            // if the selected feature is a point, change the geometry with the map point
            selectedFeature.geometry = mapPoint;
        }
        // update the selected feature with the new geometry
        selectedFeature.featureTable.updateFeature(selectedFeature);
        clearSelection();
    }
}
