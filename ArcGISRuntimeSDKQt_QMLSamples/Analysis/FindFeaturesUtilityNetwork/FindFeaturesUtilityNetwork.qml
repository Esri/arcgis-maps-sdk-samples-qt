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
    property list<UtilityElement> elementGroup
    property UtilityTerminal terminal
    property ArcGISFeature feature
    property Point clickPoint
    property list<Feature> junctions;
    property list<Feature> devices;
    property list<UtilityElement> elementLines;
    property list<UtilityElement> elementJunctions;
    property list<UtilityElement> elementDevices;


    property UtilityTraceResult myTraceResult

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

                // feature table
                ServiceFeatureTable {
                    id: lineLayerFeatureTable
                    url: featureLayerUrl + "/115"
                }

                renderer: SimpleRenderer {
                    symbol: featureLineSymbol
                }

                onSelectedFeaturesStatusChanged: {
                    if( (selectedFeaturesStatus !== Enums.TaskStatusCompleted) && (electricDevicelayer.selectFeaturesStatus !== Enums.TaskStatusCompleted))
                        return;
                    busy.visible = false;
                }

                //remove later
                onLoadStatusChanged: {
                    if(loadStatus !== Enums.LoadStatusLoaded)
                        return
                    console.log("service feature table load status: " + loadStatus);
                }
            }

            FeatureLayer {
                id: electricDevicelayer

                // feature table
                ServiceFeatureTable {
                    id: featureTable
                    url: featureLayerUrl + "/100"
                }

                onSelectedFeaturesStatusChanged: {
                    if( (selectedFeaturesStatus !== Enums.TaskStatusCompleted) && (lineLayer.selectFeaturesStatus !== Enums.TaskStatusCompleted))
                        return;
                    busy.visible = false;
                }

                //remove later
                onLoadStatusChanged: {
                    if(loadStatus !== Enums.LoadStatusLoaded)
                        return
                    console.log("service feature table load status: " + loadStatus);
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
            console.log("Oy");

            var results = mapView.identifyLayersResults;
            console.log(results.length);
            // make sure there are results do later
            var result = results[0];
            if ( !result )
                return;
//            var geoElements = result.geoElements;
            feature = result.geoElements[0];

            var isAddingStart = startingLocBtn.checked;

            utilityNetworkSource = utilityNetwork.definition.networkSource(feature.featureTable.tableName);
            console.log("table name: " + feature.featureTable.tableName);
            // work around for features for elements result (doesn't work 100%)
            utilityNetworkSource.featureTable.load();

            if (utilityNetworkSource.sourceType === Enums.UtilityNetworkSourceTypeJunction) {
                console.log("Junc");
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
                    console.log("id1: %1 - id2: %2".arg(terminals[0].terminalId).arg(terminals[1].terminalId));
                    console.log(terminals.length);
                } else if (terminals.length === 1) {
                    element = utilityNetwork.createElementWithArcGISFeature(feature, terminals[0]);
                    console.log(element.globalId);
                }

            } else if (utilityNetworkSource.sourceType === Enums.UtilityNetworkSourceTypeEdge) {
                console.log("Edge");

                element = utilityNetwork.createElementWithArcGISFeature(feature);

                if (feature.geometry.geometryType === Enums.GeometryTypePolyline) {
                    var line = GeometryEngine.removeZ(feature.geometry);
                    console.log(clickPoint.x);
                    element.fractionAlongEdge = GeometryEngine.fractionAlong(line, clickPoint, -1);
                    console.log("fraction along edge: %1".arg(element.fractionAlongEdge))
                }
            }

            if ( !element )
                return;

            if ( startingLocBtn.checked ) {
                startingLocations.push(element);
                params.startingLocations = startingLocations;
                console.log("startingLocations params length: " + params.startingLocations.length);

            } else {
                barriers.push(element);
                params.barriers = barriers;
                console.log("barriers params length: " + params.barriers.length);
            }

            var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                    geometry: clickPoint,
                                                                    symbol: startingLocBtn.checked ? startingPointSymbol : barrierPointSymbol
                                                                });
            unGraphicsOverlay.graphics.append(graphic);
            console.log(unGraphicsOverlay.graphics.count);
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

        onLoadStatusChanged: {
            console.log("Load status changed: %1".arg(loadStatus));


        }

        onTraceStatusChanged: {
            if (traceStatus !== Enums.TaskStatusCompleted)
                return;

            if (traceResult.count === 0)
                return;

            console.log("completed trace");


            myTraceResult = traceResult.get(0);
            var resultElements = myTraceResult.elements;
            console.log(resultElements.length);
//            var x = {};
//            for (let j = 0; j < resultElements.length; j++) {
//                console.log(resultElements[j].networkSource.name);
//                if (resultElements[j].networkSource.name === "Electric Distribution Device") {
//                    elementDevices.push(resultElements[j]);

//                } else if (resultElements[j].networkSource.name === "Electric Distribution Line") {
//                    elementLines.push(resultElements[j]);

//                } else if (resultElements[j].networkSource.name === "Electric Distribution Junction") {
//                    elementJunctions.push(resultElements[j]);

//                }
//            }

//            x["Electric Distribution Device"] = elementDevices;
//            x["Electric Distribution Line"] = elementLines;
//            x["Electric Distribution Junction"] = elementJunctions;


//            console.log(x["Electric Distribution Device"][0].networkSource.name);




//            featuresForElements(elementLines);
//            featuresForElements(elementJunctions);
//            featuresForElements(elementDevices);
            featuresForElements(resultElements);
        }

        onFeaturesForElementsStatusChanged: {
            if (featuresForElementsStatus !== Enums.TaskStatusCompleted)
                return;

            if (featuresForElementsResult.count < 1)
                return;

            console.log("features for elements result: " + featuresForElementsResult.count)

            console.log("length: " + featuresForElementsResult.features.length);

            console.log(lineLayer.featureTable.displayName);
            console.log(electricDevicelayer.featureTable.displayName);

//            for (var i = 0; i < featuresForElementsResult.features.length; i++) {
//                console.log(featuresForElementsResult.features[i].featureTable.tableName);
////                if(featuresForElementsResult.features[i].featureTable.tableName === lineLayer.featureTable.tableName) {
////                    layerLines.push(featuresForElementsResult.features[i]);
////                } else if (featuresForElementsResult.features[i].featureTable.tableName === electricDevicelayer.featureTable.tableName) {
////                    layerDevices.push(featuresForElementsResult.features[i]);
////                }

//                if (featuresForElementsResult.features[i].featureTable.tableName === lineLayer.featureTable.tableName) {
//                    layerLines.push(featuresForElementsResult.features[i]);
//                } else if (featuresForElementsResult.features[i].featureTable.tableName === electricDevicelayer.featureTable.tableName) {
//                    layerDevices.push(featuresForElementsResult.features[i]);
//                }
//            }
//            console.log(i);

            //simpler than above for breaking them down by table name

//            if (featuresForElementsResult.features[0].featureTable.tableName === lineLayer.featureTable.tableName) {
//                lineLayer.selectFeatures(featuresForElementsResult.features);
//            } else if (featuresForElementsResult.features[0].featureTable.tableName === electricDevicelayer.featureTable.tableName) {
//                electricDevicelayer.selectFeatures(featuresForElementsResult.features);
//            }

            lineLayer.selectFeatures(featuresForElementsResult.features);
            electricDevicelayer.selectFeatures(featuresForElementsResult.features);
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
            console.log("accepted :%1".arg(terminalSelection.currentIndex));
            terminal = terminals[terminalSelection.currentIndex];
            element = utilityNetwork.createElementWithArcGISFeature(feature, terminal);
            console.log(element.objectId);
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
