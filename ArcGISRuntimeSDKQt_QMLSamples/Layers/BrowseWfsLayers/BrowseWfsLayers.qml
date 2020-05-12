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
import Esri.ArcGISRuntime 100.9
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
    property bool swapAxis: false

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}

            onComponentCompleted: createWfsService();
        }

        BusyIndicator {
            id: loadingIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            running: false;
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

                ComboBox {
                    id: layersComboBox
                    model: myWfsListModel
                    currentIndex: 0
                    Layout.fillWidth: true
                    Layout.margins: 3
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    id: loadSelectedLayerBtn
                    text: qsTr("Load Selected Layer")
                    Layout.fillWidth: true
                    Layout.margins: 3
                    onClicked: {
                        swapAxis = false;
                        createWfsFeatureTable();
                    }
                }

                Button {
                    id: swapAxisOrder
                    text: qsTr("Swap Coordinate Order")
                    Layout.margins: 3
                    Layout.fillWidth: true
                    onClicked:{
                        swapAxis = !swapAxis;
                        createWfsFeatureTable();
                    }
                }
            }
        }
    }

    function createWfsService() {
        // create WFS Service
        const service = ArcGISRuntimeEnvironment.createObject("WfsService", {url: serviceUrl});

        // once WFS service is laoded create ListModel from Layer titles for ComboBox and create WFS Feature Table
        service.loadStatusChanged.connect(()=> {
            if (service.loadStatus === Enums.LoadStatusLoaded) {
                wfsLayersInfoList = service.serviceInfo.layerInfos;

                //once loaded populate myWfsListModel with titles from the service to display in a comboBox
                for (let i in wfsLayersInfoList){
                    const data = {"title": wfsLayersInfoList[i].title};
                    myWfsListModel.append(data);
                }
            }
        });
        service.load();
    }

    function createWfsFeatureTable() {
        // enable busy indicator
        loadingIndicator.running = true;
        // clear previous layer
        mapView.map.operationalLayers.clear();
        // create WFS Feature Table from selected layer
        wfsFeatureTable = ArcGISRuntimeEnvironment.createObject("WfsFeatureTable", {layerInfo: wfsLayersInfoList[layersComboBox.currentIndex]});

        // Set feature request mode to manual - only manual is supported at v100.5.
        // In this mode, you must manually populate the table - panning and zooming
        // won't request features automatically.
        wfsFeatureTable.featureRequestMode = Enums.FeatureRequestModeManualCache;

        if (swapAxis) {
            wfsFeatureTable.axisOrder = Enums.OgcAxisOrderSwap;
        } else {
            wfsFeatureTable.axisOrder = Enums.OgcAxisOrderNoSwap;
        }

        // once WFS Feature Table is loaded, populate the table and add the layer to the map
        wfsFeatureTable.loadStatusChanged.connect(()=> {
            if (wfsFeatureTable.loadStatus !== Enums.LoadStatusLoaded)
                return;

            populateWfsFeatureTable();
        });
        wfsFeatureTable.load();
    }

    function populateWfsFeatureTable() {
        // Create query parameters
        const params = ArcGISRuntimeEnvironment.createObject("QueryParameters", {
                                                                 geometry: mapView.visibleArea.extent,
                                                                 spatialRelationship: Enums.SpatialRelationshipIntersects
                                                             });

        wfsFeatureTable.populateFromServiceStatusChanged.connect(()=> {
            if(wfsFeatureTable.populateFromServiceStatus !== Enums.TaskStatusCompleted)
                return;

            addFeatureLayerToMap();

            // set viewpoint to extent of selected layer
            mapView.setViewpointGeometry(wfsFeatureTable.extent);
        });
        // Populate features based on query
        wfsFeatureTable.populateFromService(params, false, ["*"]);
    }

    function addFeatureLayerToMap() {
        let simpleSymbol;
        let symbolLine;

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
        case Enums.GeometryTypeEnvelope:
        case Enums.GeometryTypeEnvelope:
            console.debug("Error! No Renderer defined for this GeometryType");
            return;
        }

        const simpleRenderer = ArcGISRuntimeEnvironment.createObject("SimpleRenderer",{symbol: simpleSymbol});

        const featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {
                                                                       featureTable: wfsFeatureTable,
                                                                       renderer: simpleRenderer
                                                                   });
        // add the layer to the map
        mapView.map.operationalLayers.append(featureLayer);
        // disable busy indicator
        loadingIndicator.running = false;
    }
}
