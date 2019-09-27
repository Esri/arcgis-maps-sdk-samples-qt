// [WriteFile Name=FindFeaturesUtilityNetwork, Category=Analysis]
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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.7

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url featureLayerUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"
    property UtilityElement element
    property UtilityNetworkSource utilityNetworkSource
    property list<UtilityTerminal> startingLocations
    property list<UtilityTerminal> barriers
    property list<UtilityTerminal> terminals
    property UtilityTerminal terminal
    property ArcGISFeature feature
    property Point clickPoint
    property var deviceObjIds: []
    property var lineObjIds: []


    property UtilityElementTraceResult myTraceResult

    property FeatureLayer selectionLayer: null


    MapView {
        id: mapView
        anchors.fill: parent
        
        Map {
            BasemapStreetsNightVector {}
            
            ViewpointExtent {
                Envelope {
                    xMin: -9813547.35557238
                    yMin: 5129980.36635111
                    xMax: -9813185.0602376
                    yMax: 5130215.41254146
                    spatialReference: SpatialReference.createWebMercator();
                }
            }

            FeatureLayer {
                id: lineLayer

                ServiceFeatureTable {
                    url: featureLayerUrl + "/115"
                }

                renderer: SimpleRenderer {
                    symbol: featureLineSymbol
                }

                onSelectFeaturesStatusChanged: {
                    if( (selectedFeaturesStatus !== Enums.TaskStatusCompleted) && (deviceLayer.selectFeaturesStatus !== Enums.TaskStatusCompleted))
                        return;
                    busy.visible = false;
                }
            }

            FeatureLayer {
                id: deviceLayer

                // feature table
                ServiceFeatureTable {
                    url: featureLayerUrl + "/100"
                }

                onSelectFeaturesStatusChanged: {
                    if( (selectedFeaturesStatus !== Enums.TaskStatusCompleted) && (lineLayer.selectFeaturesStatus !== Enums.TaskStatusCompleted))
                        return;
                    busy.visible = false;
                }
            }
        }

        GraphicsOverlay {
            id: unGraphicsOverlay
        }

        onMouseClicked: {
            var screenX = mouse.x;
            var screenY = mouse.y;
            var tolerance = 10;
            var returnPopups = false;
            clickPoint = mouse.mapPoint;
            mapView.identifyLayers(screenX, screenY, tolerance, returnPopups);
        }

        onIdentifyLayersStatusChanged : {
            if (identifyLayersStatus !== Enums.TaskStatusCompleted)
                return;

            var results = mapView.identifyLayersResults;
            // make sure there are results do later
            var result = results[0];
            if ( !result )
                return;
            feature = result.geoElements[0];

            var isAddingStart = startingLocBtn.checked;

            utilityNetworkSource = utilityNetwork.definition.networkSource(feature.featureTable.tableName);

            if (utilityNetworkSource.sourceType === Enums.UtilityNetworkSourceTypeJunction) {
                var assetGroupFieldName = feature.featureTable.subtypeField;
                var assetGroupCode = feature.attributes.attributeValue(assetGroupFieldName);

                var assetGroup;
                for (let i = 0; i < utilityNetworkSource.assetGroups.length; i++) {
                    if (utilityNetworkSource.assetGroups[i].code === assetGroupCode) {
                        assetGroup = utilityNetworkSource.assetGroups[i];
                        break;
                    }
                }

                var assetTypeCode = feature.attributes.attributeValue("assettype");

                var assetType;
                for (let j = 0; j < assetGroup.assetTypes.length; j++) {
                    if (assetGroup.assetTypes[j].code === assetTypeCode) {
                        assetType = assetGroup.assetTypes[j];
                        break;
                    }
                }
                terminals = assetType.terminalConfiguration.terminals;

                if (terminals.length > 1) {
                    multiTerminal.visible = true;
                } else if (terminals.length === 1) {
                    element = utilityNetwork.createElementWithArcGISFeature(feature, terminals[0]);
                }

            } else if (utilityNetworkSource.sourceType === Enums.UtilityNetworkSourceTypeEdge) {

                element = utilityNetwork.createElementWithArcGISFeature(feature);

                if (feature.geometry.geometryType === Enums.GeometryTypePolyline) {
                    var line = GeometryEngine.removeZ(feature.geometry);
                    element.fractionAlongEdge = GeometryEngine.fractionAlong(line, clickPoint, -1);
                }
            }

            if ( !element )
                return;

            if ( startingLocBtn.checked ) {
                startingLocations.push(element);
                params.startingLocations = startingLocations;

            } else {
                barriers.push(element);
                params.barriers = barriers;
            }

            var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                    geometry: clickPoint,
                                                                    symbol: startingLocBtn.checked ? startingPointSymbol : barrierPointSymbol
                                                                });
            unGraphicsOverlay.graphics.append(graphic);
        }

    }

    SimpleLineSymbol {
        id: featureLineSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: "DarkCyan"
        width: 3
    }

    SimpleMarkerSymbol {
        id: startingPointSymbol
        style: Enums.SimpleMarkerSymbolStyleCross
        color: "green"
        size: 20
    }

    SimpleMarkerSymbol {
        id: barrierPointSymbol
        style: Enums.SimpleMarkerSymbolStyleX
        color: "Red"
        size: 20
    }

    UtilityNetwork {
        id: utilityNetwork
        url: featureLayerUrl
        initMap: mapView.map

        onTraceStatusChanged: {
            if (traceStatus !== Enums.TaskStatusCompleted)
                return;

            if (traceResult.count === 0) {
                busy.visible = false;
                return;
            }


            myTraceResult = traceResult.get(0);
            var resultElements = myTraceResult.elements;
            for (let i = 0; i < resultElements.length; i++) {
                if (resultElements[i].networkSource.name === "Electric Distribution Device") {
                    deviceObjIds.push(resultElements[i].objectId);
                } else if (resultElements[i].networkSource.name === "Electric Distribution Line") {
                    lineObjIds.push(resultElements[i].objectId);
                }
            }

            let deviceParams = ArcGISRuntimeEnvironment.createObject("QueryParameters");
            let lineParams = ArcGISRuntimeEnvironment.createObject("QueryParameters");
            deviceParams.objectIds = deviceObjIds;
            lineParams.objectIds = lineObjIds;

            deviceLayer.selectFeaturesWithQuery(deviceParams, Enums.SelectionModeAdd);
            lineLayer.selectFeaturesWithQuery(lineParams, Enums.SelectionModeAdd);
        }

        onComponentCompleted: load();
    }

    UtilityTraceParameters {
        id: params
    }

    Dialog {
        id: multiTerminal
        modal: true
        standardButtons: Dialog.Ok
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        ColumnLayout {
            Text {
                text: qsTr("Select the terminal for this junction.")
                Layout.alignment: Qt.AlignHCenter
            }

            ComboBox {
                id: terminalSelection
                model: ["High", "Low"]
                Layout.alignment: Qt.AlignHCenter
            }
        }

        onAccepted: {
            terminal = terminals[terminalSelection.currentIndex];
            element = utilityNetwork.createElementWithArcGISFeature(feature, terminal);
        }

    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        visible: false
    }

    Rectangle {
        id: backBox
        anchors {
            left: parent.left
            top: parent.top
            margins: 3
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "lightgrey"
        opacity: 0.8
        radius: 5

        GridLayout {
            columns: 2
            rows: 2
            flow: GridLayout.LeftToRight
//        ColumnLayout {
            RadioButton {
                id: startingLocBtn
                checked: true
                text: qsTr("Add starting locations")
            }

            RadioButton {
                id: barriersBtn
                text: qsTr("Add barriers")
            }

            Button {
                id: resetBtn
                text: qsTr("Reset")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    params.barriers = null;
                    barriers = null;
                    params.startingLocations = null;
                    startingLocations = null;
                    mapView.graphicsOverlays.get(0).graphics.clear();
                    mapView.map.operationalLayers.forEach(function(element){
                        element.clearSelection();
                    });
                }
            }

            Button {
                id: traceBtn
                text: qsTr("Trace")
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    busy.visible = true;
                    utilityNetwork.trace(params);
                }
            }
        }
    }
}
