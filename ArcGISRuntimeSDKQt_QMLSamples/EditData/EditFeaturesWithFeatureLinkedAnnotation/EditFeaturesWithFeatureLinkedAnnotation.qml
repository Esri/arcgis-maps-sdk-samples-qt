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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data"
    property var selectedFeature: null
    property var pointsFeatureLayer: null
    property var linesFeatureLayer: null
    readonly property string pointsFeatureLayerName: "Loudoun_Address_Points_1"
    readonly property string linesFeatureLayerName: "ParcelLines_1"
    readonly property string pointsAnnoLayerName: "Loudoun_Address_PointsAnno_1"
    readonly property string linesAnnoLayerName: "ParcelLinesAnno_1"
    property string addressText: ""
    property string streetNameText: ""

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapLightGrayCanvasVector {}

            // Set the initialViewpoint property to a ViewpointCenter object
            initialViewpoint: viewpoint
        }

        onMouseClicked: {
            let clickPoint = mapView.screenToLocation(mouse.x, mouse.y);

            if (updateWindow.visible)
                updateWindow.visible = false;

            if (selectedFeature) {
                moveVertex(clickPoint);
            } else {
                mapView.identifyLayers(mouse.x,  mouse.y, 10, false);
            }
        }

        onIdentifyLayersStatusChanged: {
            if (mapView.identifyLayersStatus !== Enums.TaskStatusCompleted)
                return;

            const results = mapView.identifyLayersResults;

            for (let j = 0; j < results.length; j++) {

                const layerContent = identifyLayersResults[j].layerContent;

                selectedFeature = results[j].geoElements[0];
                const geometry = selectedFeature.geometry;

                if (geometry.geometryType === Enums.GeometryTypePoint) {

                    addressText = selectedFeature.attributes.attributeValue("AD_ADDRESS");
                    streetNameText = selectedFeature.attributes.attributeValue("ST_STR_NAM");
                    pointsFeatureLayer.selectFeature(selectedFeature);
                    updateWindow.visible = true;
                } else if (geometry.geometryType === Enums.GeometryTypePolyline ) {

                    const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {
                                                                                      geometry: geometry,
                                                                                      spatialReference: Factory.SpatialReference.createWgs84()
                                                                                  });

                    if (polylineBuilder.parts.part(0).segmentCount > 1) {
                        print("only straight lines");
                        clearSelection();
                        return;
                    }

                    linesFeatureLayer.selectFeature(selectedFeature);
                }
            }
        }
    }

    // Create the intial Viewpoint
    ViewpointCenter {
        id: viewpoint
        // Specify the center Point
        center: Point {
            x: -77.41589
            y: 39.0204
            spatialReference: Factory.SpatialReference.createWgs84()
        }
        // Specify the scale
        targetScale: 2257
    }

    Geodatabase {
        property var gdbLayers: []

        id: geodatabase
        path: dataPath + "/geodatabase/loudoun_anno.geodatabase"

        onLoadStatusChanged: {
            if (Enums.LoadStatusLoaded !== geodatabase.loadStatus)
                return;

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
        width: childrenRect.width
        height: childrenRect.height
        anchors.centerIn: parent
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
                text: "Update Attributes"
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
                height: childrenRect.height
                Layout.columnSpan: 2
                spacing: 5

                Button {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignRight
                    text: "Update"
                    onClicked: {
                        updateWindow.visible = false;

                        if (!selectedFeature)
                            return;

                        selectedFeature.attributes.replaceAttribute("AD_ADDRESS", attAddressTextField.text);
                        selectedFeature.attributes.replaceAttribute("ST_STR_NAM", attStreetTextField.text);
                        selectedFeature.featureTable.updateFeature(selectedFeature);
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: "Cancel"
                    // once the cancel button is clicked, hide the window
                    onClicked: updateWindow.visible = false;
                }
            }
        }
    }


    function clearSelection() {
        pointsFeatureLayer.clearSelection();
        linesFeatureLayer.clearSelection();

        selectedFeature = null;
    }

    function moveVertex(mapPoint) {
        const geometry = selectedFeature.geometry;
        const workingPoint = GeometryEngine.project(mapPoint, geometry.spatialReference);

        if (geometry.geometryType === Enums.GeometryTypePolyline) {
            const nearestVertex = GeometryEngine.nearestVertex(geometry, workingPoint);


            const polylineBuilder = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {
                                                                              geometry: geometry,
                                                                              spatialReference: Factory.SpatialReference.createWgs84()
                                                                          });

            const part = polylineBuilder.parts.part(nearestVertex.partIndex);

            part.removePoint(nearestVertex.pointIndex);
            part.addPoint(workingPoint);
            selectedFeature.geometry = polylineBuilder.geometry;
            selectedFeature.featureTable.updateFeature(selectedFeature);
            clearSelection();



        } else if ( geometry.geometryType === Enums.GeometryTypePoint ) {
            selectedFeature.geometry = mapPoint;
            selectedFeature.featureTable.updateFeature(selectedFeature);
            clearSelection();
        }
    }

    function updateSelectedFeature(addText, streetText) {



//        if (!m_selectedFeature)
//          return;

//        m_selectedFeature->attributes()->replaceAttribute("AD_ADDRESS", address);
//        m_selectedFeature->attributes()->replaceAttribute("ST_STR_NAM", streetName);
//        m_selectedFeature->featureTable()->updateFeature(m_selectedFeature);
    }
}
