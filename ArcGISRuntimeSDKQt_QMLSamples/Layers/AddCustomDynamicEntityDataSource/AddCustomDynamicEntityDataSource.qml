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
import Esri.ArcGISRuntime

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            initBasemapStyle: Enums.BasemapStyleArcGISTopographic

            DynamicEntityLayer {
                dataSource: CustomDynamicEntityDataSource {
                    onConnectAsync: {
                        addObservation(ArcGISRuntimeEnvironment.createObject("Point", {x: 0, y: 0, spatialReference: Factory.SpatialReference.createWgs84()}), {});
                    }

                    onDisconnectAsync: {

                    }

                    onLoadAsync: {
                        const fieldList = [ ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "MMSI", length: 256}),
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
                                            ArcGISRuntimeEnvironment.createObject("Field", {fieldType: Enums.FieldTypeText, name: "globalid", length: 256}) ];


                        return ArcGISRuntimeEnvironment
                        .createObject("DynamicEntityDataSourceInfo",
                                      {
                                          entityIdFieldName: "MMSI",
                                          fields: fieldList,
                                          spatialReference: Factory.SpatialReference.createWgs84()
                                      });
                    }
                }
            }
        }
    }
}
