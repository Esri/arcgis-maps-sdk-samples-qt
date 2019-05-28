// [WriteFile Name=BrowseWfsLayers, Category=Layers]
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
import Esri.ArcGISRuntime 100.6
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url serviceUrl: "https://dservices2.arcgis.com/ZQgQTuoyBrtmoGdP/arcgis/services/Seattle_Downtown_Features/WFSServer?service=wfs&request=getcapabilities"
    property ListModel myWfsListModel: ListModel{}
    property var wfsLayersInfoList: []
    property var wfsFeatureTable

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            // Set initial viewpoint on the map
            ViewpointExtent {
                Envelope {
                    id: initialExtent
                    xMin: -122.341581
                    yMin: 47.613758
                    xMax: -122.332662
                    yMax: 47.617207
                    spatialReference: SpatialReference { wkid: 4326 }
                }
            }
            onComponentCompleted: createWfsService();
        }

        Rectangle {
            anchors {
                margins: 5
                right: parent.right
                top: parent.top
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#000000"
            opacity: .75
            radius: 5

            ColumnLayout {
                Text {
                    color: "#ffffff"
                    text: qsTr("Pick a WFS Layer")
                    Layout.margins: 3
                    Layout.alignment: Qt.AlignHCenter
                }

                Row {
                    Layout.margins: 3
                    CheckBox {
                        id: axisOrderBox
                        checked: false
                        onCheckStateChanged: createWfsFeatureTable();
                    }
                    Text {
                        text: qsTr("Swap Coordinate Order")
                        height: axisOrderBox.height
                        verticalAlignment: Text.AlignVCenter
                        color: "#ffffff"
                    }
                }

                ComboBox {
                    id: layersComboBox
                    model: myWfsListModel
                    currentIndex: 0
                    Layout.fillWidth: true
                    Layout.margins: 3
                    Layout.alignment: Qt.AlignHCenter
                    onCurrentIndexChanged: createWfsFeatureTable();
                }
            }
        }

        onNavigatingChanged: {
            if (navigating)
                return;

            populateWfsFeatureTable();
        }
    }

    function createWfsService() {

        // create WFS Service
        var service = ArcGISRuntimeEnvironment.createObject("WfsService", {url: serviceUrl});

        // once WFS service is laoded create ListModel from Layer titles for ComboBox and create WFS Feature Table
        service.loadStatusChanged.connect(function() {
            if (service.loadStatus === Enums.LoadStatusLoaded) {
                wfsLayersInfoList = service.serviceInfo.layerInfos;

                //once loaded populate myWfsListModel with titles from the service to display in a comboBox
                for(var i in wfsLayersInfoList){
                    var data = {'title': wfsLayersInfoList[i].title};
                    myWfsListModel.append(data);
                }
                createWfsFeatureTable();
            }
        });
        service.load();
    }

    function createWfsFeatureTable() {
        // create WFS Feature Table from selected layer
        wfsFeatureTable = ArcGISRuntimeEnvironment.createObject("WfsFeatureTable", {layerInfo: wfsLayersInfoList[layersComboBox.currentIndex]});

        // Set feature request mode to manual - only manual is supported at v100.5.
        // In this mode, you must manually populate the table - panning and zooming
        // won't request features automatically.
        wfsFeatureTable.featureRequestMode = Enums.FeatureRequestModeManualCache;

        if(axisOrderBox.checkState === Qt.Checked) {
            wfsFeatureTable.axisOrder = Enums.OgcAxisOrderSwap;
        } else {
            wfsFeatureTable.axisOrder = Enums.OgcAxisOrderNoSwap;
        }

        // once WFS Feature Table is loaded, populate the table and add the layer to the map
        wfsFeatureTable.loadStatusChanged.connect(function() {
            if (wfsFeatureTable.loadStatus !== Enums.LoadStatusLoaded)
                return;

            populateWfsFeatureTable();
            addFeatureLayerToMap();
        });
        wfsFeatureTable.load();
    }

    function populateWfsFeatureTable() {
        // Create query parameters
        var params = ArcGISRuntimeEnvironment.createObject("QueryParameters", {
                                                               geometry: mapView.visibleArea.extent,
                                                               spatialRelationship: Enums.SpatialRelationshipIntersects
                                                           });
        // Populate features based on query
        wfsFeatureTable.populateFromService(params, false, ["*"]);
    }

    function addFeatureLayerToMap() {
        var simpleSymbol;
        var symbolLine;

        // appropriate symbology for each corresponding geometry type
        switch (wfsFeatureTable.geometryType) {
        case Enums.GeometryTypePoint:
            simpleSymbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {
                                                                     color: "green",
                                                                     outline: symbolLine,
                                                                     size: 4,
                                                                     style: Enums.SimpleMarkerSymbolStyleCircle
                                                                 });
            break;
        case Enums.GeometryTypePolygon:
            simpleSymbol = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol", {
                                                                     color: "blue",
                                                                     width: 3,
                                                                     style: Enums.SimpleLineSymbolStyleSolid
                                                                 });
            break;
        case Enums.GeometryTypePolyline:
            simpleSymbol = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol", {
                                                                     color: "red",
                                                                     width: 3,
                                                                     style: Enums.SimpleLineSymbolStyleSolid
                                                                 });
            break;
        case Enums.GeometryTypeUnknown:
            return;
        case Enums.GeometryTypeEnvelope:
            return;
        case Enums.GeometryTypeEnvelope:
            return;
        }

        var simpleRenderer = ArcGISRuntimeEnvironment.createObject("SimpleRenderer",{symbol: simpleSymbol});

        var featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {
                                                                  featureTable: wfsFeatureTable,
                                                                  renderer: simpleRenderer
                                                              });
        mapView.map.operationalLayers.clear();
        // add the layer to the map
        mapView.map.operationalLayers.append(featureLayer);
    }
}
