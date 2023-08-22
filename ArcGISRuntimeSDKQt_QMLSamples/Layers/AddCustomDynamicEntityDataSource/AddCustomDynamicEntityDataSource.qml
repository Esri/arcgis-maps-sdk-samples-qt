// [WriteFile Name=AddCustomDynamicEntityDataSource, Category=Layers]
// [Legal]
// Copyright 2023 Esri.

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

import QtQuick
import Esri.ArcGISExtras
import Esri.ArcGISRuntime
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property list<string> observations: []
    property DynamicEntity dynamicEntity: null

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISOceans

            initialViewpoint: ViewpointCenter {
                center: Point {
                    x: -123.657
                    y: 47.984
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 3e6
            }

            DynamicEntityLayer {
                id: dynamicEntityLayer

                trackDisplayProperties {
                    showPreviousObservations: true
                    showTrackLine: true
                    maximumObservations: 20
                }

                LabelDefinition {
                    expression: SimpleLabelExpression {
                        expression: "[VesselName]"
                    }
                    textSymbol: TextSymbol {
                        color: "red"
                        size: 12
                    }

                    placement: Enums.LabelingPlacementPointAboveCenter
                }

                labelsEnabled: true

                dataSource: CustomDynamicEntityDataSource {
                    id: customDynamicEntityDataSource

                    purgeOptions.maximumObservationsPerTrack: 20

                    onConnectAsync: {
                        timer.start();
                        connectAsyncCompleted();
                    }

                    onDisconnectAsync: {
                        disconnectAsyncCompleted();
                    }

                    onLoadAsync: {
                        const fieldList = [ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "MMSI", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "BaseDateTime", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "LAT", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "LONG", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "SOG", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "COG", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "Heading", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "VesselName", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "IMO", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "CallSign", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "VesselType", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "Status", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "Length", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeFloat64, name: "Width", length: 8}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "Cargo", length: 256}),
                                           ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "globalid", length: 256})];

                        const dynamicEntityDataSourceInfo = ArcGISRuntimeEnvironment
                        .createObject("DynamicEntityDataSourceInfo",
                                      {
                                          entityIdFieldName: "MMSI",
                                          fields: fieldList,
                                          spatialReference: Factory.SpatialReference.createWgs84()
                                      });

                        loadAsyncCompleted(dynamicEntityDataSourceInfo);
                    }
                }
            }
        }

        onMouseClicked: (mouseClick) => {
                            mapView.calloutData.visible = false;
                            dynamicEntity = null;
                            mapView.identifyLayer(dynamicEntityLayer, mouseClick.x, mouseClick.y, 10, false);
                            mapView.calloutData.location = mouseClick.mapPoint
                        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus !== Enums.TaskStatusCompleted)
                return;

            if (identifyLayerResult.geoElements.length === 0)
                return;

            dynamicEntity = identifyLayerResult.geoElements[0].dynamicEntity();
            dynamicEntity.onDynamicEntityChanged.connect(updateCallout);
        }
    }

    function updateCallout() {
        if (!dynamicEntity)
            return;

        const observation = dynamicEntity.latestObservation();
        let attributes = observation.attributes

        mapView.calloutData.geoElement = observation;
        const calloutText = "Vessel Name: " + attributes.attributeValue("VesselName") + "\n" +
                          "Call Sign: " + attributes.attributeValue("CallSign") + "\n" +
                          "Course over Ground: " + attributes.attributeValue("COG") + "\n" +
                          "Speed over Ground: " + attributes.attributeValue("SOG") + "\n"
        mapView.calloutData.detail = calloutText;
        mapView.calloutData.location = observation.geometry;
        mapView.calloutData.visible = true;
    }

    Callout {
        id: callout
        height: 120
        calloutData: mapView.calloutData
        accessoryButtonVisible: false
    }

    FileFolder {
        id: jsonFile
        url: "qrc:/Samples/Layers/AddCustomDynamicEntityDataSource/"

        property var observationList: []

        Component.onCompleted: {
            observationList = readTextFile("AIS_MarineCadastre_SelectedVessels_CustomDataSource.json").split("\n");
        }
    }

    Timer {
        id: timer
        interval: 10
        repeat: true
        property int i: 0
        onTriggered: {
            const line = JSON.parse(jsonFile.observationList[i]);
            const geometry = line["geometry"];
            const attributes = line["attributes"];
            const point = ArcGISRuntimeEnvironment.createObject("Point", {x: geometry["x"], y: geometry["y"], spatialReference: Factory.SpatialReference.createWgs84()});

            customDynamicEntityDataSource.addObservation(point, attributes);

            i++;
            if (i >= jsonFile.observationList.length)
                i = 0;
        }
    }
}
