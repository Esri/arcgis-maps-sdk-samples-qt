// [WriteFile Name=DisplayUtilityAssociations, Category=Analysis]
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
import Esri.ArcGISRuntime 100.8

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url featureServerUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"
    readonly property int maxScale: 2000
    readonly property int viewpointScale: 40

    MapView {
        id: mapView
        anchors.fill: parent

        Rectangle {
            id: backgroundRect
            border.color: "black"
            border.width: 1
            width: connectivityLabel.width * 1.5
            height: width/2
            anchors {
                top: parent.top
                left: parent.left
                margins: 20
            }

            ColumnLayout {
                anchors.horizontalCenter: parent.horizontalCenter
                Label {
                    text: "Utility association types"
                    Layout.alignment: Qt.AlignHCenter
                }

                RowLayout {
                    Layout.leftMargin: 5
                    Image {
                        id: attachmentImage
                        fillMode: Image.PreserveAspectFit
                    }
                    Label {
                        id: attachmentLabel
                        text: "Attachment symbol"
                    }
                }

                RowLayout {
                    Layout.leftMargin: 5
                    Image {
                        id: connectivityImage
                    }
                    Label {
                        id: connectivityLabel
                        text: "Connectivity symbol"
                    }
                }
            }
        }

        Point {
            id: centerPoint
            x: -9812698.37297436
            y: 5131928.33743317
            SpatialReference {wkid: 3857}
        }

        Map {
            id: map
            BasemapTopographicVector {}

            Component.onCompleted: {
                utilityNetwork.load();
            }

            UtilityNetwork {
                id: utilityNetwork
                url: featureServerUrl

                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    mapView.setViewpointCenterAndScale(centerPoint, viewpointScale);

                    let edges = [];
                    let junctions = [];

                    // get all the edges and junctions in the network
                    for (let i = 0; i < definition.networkSources.length; i++) {
                        if (definition.networkSources[i].sourceType === Enums.UtilityNetworkSourceTypeEdge) {
                            edges.push(definition.networkSources[i]);
                        } else if (definition.networkSources[i].sourceType === Enums.UtilityNetworkSourceTypeJunction) {
                            junctions.push(definition.networkSources[i]);
                        }
                    }

                    // add all edges that are not subnet lines to the map
                    for (let j = 0; j < edges.length; j++) {
                        if (edges[j].sourceUsageType !== Enums.UtilityNetworkSourceUsageTypeSubnetLine && edges[j].featureTable !== null) {
                            map.operationalLayers.append(ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: edges[j].featureTable}));
                        }
                    }

                    // add all junctions to the map
                    for (let k = 0; k < junctions.length; k++) {
                        if (junctions[k].featureTable !== null) {
                            map.operationalLayers.append(ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: junctions[k].featureTable}));
                        }
                    }

                    attachmentValue.values = [Enums.UtilityAssociationTypeAttachment];
                    connectivityValue.values = [Enums.UtilityAssociationTypeConnectivity];
                }

                onAssociationsStatusChanged: {
                    if (associationsStatus !== Enums.TaskStatusCompleted)
                        return;

                    let matchedGraphic = false;
                    for (let i = 0; i < associationsResult.length; i++) {
                        let association = associationsResult[i];
                        // check if the graphics overlay already contains the association
                        for (let j = 0; j < associationsOverlay.graphics.count; j++) {
                            if (associationsOverlay.graphics.get(j).attributes.containsAttribute("GlobalId") && associationsOverlay.graphics.get(j).attributes.attributeValue("GlobalId") === association.globalId) {
                                matchedGraphic = true;
                                break;
                            }
                        }

                        // if the association is already in the overlay, don't add a new graphic
                        if (matchedGraphic) {
                            matchedGraphic = false;
                            continue;
                        }

                        // add a graphic for the association
                        var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: association.geometry});
                        graphic.attributes.insertAttribute("GlobalId", association.globalId);
                        graphic.attributes.insertAttribute("AssociationType", association.associationType);
                        associationsOverlay.graphics.append(graphic);
                    }
                }
            }
        }

        GraphicsOverlay {
            id: associationsOverlay
            renderer: uniqueValueRenderer
        }

        // create a renderer for the associations
        UniqueValueRenderer {
            id: uniqueValueRenderer
            fieldNames: ["AssociationType"]

            UniqueValue {
                id: attachmentValue
                label: "Attachment"
                SimpleLineSymbol {
                    id: attachmentSymbol
                    style: Enums.SimpleLineSymbolStyleDot
                    color: "lime"
                    width: 5

                    // create swatch image for the legend
                    Component.onCompleted: {
                        createSwatch();
                    }
                    onSwatchImageChanged: {
                        connectivityImage.source = swatchImage;
                    }
                }
            }

            UniqueValue {
                id: connectivityValue
                label: "Connectivity"
                SimpleLineSymbol {
                    id: connectivitySymbol
                    style: Enums.SimpleLineSymbolStyleDot
                    color: "red"
                    width: 5

                    // create swatch image for the legend
                    Component.onCompleted: {
                        createSwatch();
                    }
                    onSwatchImageChanged: {
                        attachmentImage.source = swatchImage;
                    }
                }
            }
        }

        // add associations to the map view
        function addAssociations() {
            // check if outside maximum scale
            if (currentViewpointCenter.targetScale >= maxScale)
                return;

            let currentExtent = currentViewpointExtent.extent;
            if (currentExtent.empty) {
                console.warn("Extent not valid.");
                return;
            }

            utilityNetwork.associationsWithEnvelope(currentExtent);
        }

        onSetViewpointCompleted: {
           if (!succeeded)
               return;

           addAssociations();
        }

        onNavigatingChanged: {
            if (navigating)
                return;

            addAssociations();
        }
    }
}
