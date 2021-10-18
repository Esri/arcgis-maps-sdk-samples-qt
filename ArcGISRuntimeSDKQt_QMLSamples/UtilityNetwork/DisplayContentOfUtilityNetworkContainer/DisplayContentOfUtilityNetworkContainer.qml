// [WriteFile Name=DisplayContentOfUtilityNetworkContainer, Category=UtilityNetwork]
// [Legal]
// Copyright 2021 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property bool containerViewIsVisible: false
    property bool createBoundingBoxGeometry: false
    property bool returnToPreviousViewpoint: false
    property var containerElement;
    property var previousViewpoint;
    property var boundingBoxExtent;

    Credential {
        id: viewerCredential
        username: "viewer01"
        password: "I68VGU^nMurF"
    }

    // The AuthenticationManager handles credential challenges raised by the web map and the utility network
    Connections {
        id: authenticationManagerConnnections
        target: AuthenticationManager
        function onAuthenticationChallenge(challenge) {
            challenge.continueWithCredential(viewerCredential);
        }
    }

    MapView {
        id: mapView
        anchors.fill: parent

        GraphicsOverlay {
            // Add a GraphicsOverlay for displaying a container view.
            id: containerGraphicsOverlay
        }

        Map {
            id: map
            // Load a web map that includes ArcGIS Pro Subtype Group Layers with only container features visible (i.e. fuse bank, switch bank, transformer bank, hand hole and junction box)
            initUrl: "https://sampleserver7.arcgisonline.com/portal/home/item.html?id=813eda749a9444e4a9d833a4db19e1c8"

            onErrorChanged: {
                messageBoxText.text = ("Map error: " + error.message + " " + error.additionalMessage);
            }

            UtilityNetwork {
                id: utilityNetwork
                // Create and load a UtilityNetwork with the same feature service URL as the layers in the Map
                url: "https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"

                onAssociationsStatusChanged: {
                    if (associationsStatus !== Enums.TaskStatusCompleted)
                        return;

                    if (!containerViewIsVisible)
                        getContainmentAssociations(associationsResult);
                    else
                        showAttachmentAndConnectivitySymbols(associationsResult);
                }

                onFeaturesForElementsStatusChanged: {
                    if (featuresForElementsStatus !== Enums.TaskStatusCompleted)
                        return;

                    getFeaturesForElements(utilityNetwork.featuresForElementsResult);
                }

                onErrorChanged: {
                    messageBoxText.text = ("UtilityNetork error: " + error.message + " " + error.additionalMessage);
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    utilityNetwork.load();
                }
            }
        }

        onErrorChanged: {
            messageBoxText.text = ("MapView error: " + error.message + " " + error.additionalMessage);
        }

        onMouseClicked: identifyLayers(mouse.x, mouse.y, 5, false);

        onIdentifyLayersStatusChanged: {
            if (mapView.identifyLayersStatus !== Enums.TaskStatusCompleted ||
                    utilityNetwork.associationsStatus === Enums.TaskStatusInProgress ||
                    utilityNetwork.featuresForElementsStatus === Enums.TaskStatusInProgress ||
                    containerViewIsVisible)
                return;

            getContainerElementFromIdentifiedFeature(mapView.identifyLayersResults);
        }

        onSetViewpointCompleted: {
            if (createBoundingBoxGeometry) {
                createBoundingBoxGeometry = false;
                boundingBoxExtent = mapView.currentViewpointExtent.extent;
                returnToPreviousViewpoint = true;
                containerGraphicsOverlay.graphics.append(ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: boundingBoxExtent, symbol: boundingBoxSymbol}));
                mapView.setViewpointGeometryAndPadding(containerGraphicsOverlay.extent, 100);
            }
        }

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

    }

    GraphicsOverlay {
        id: containerBoxGraphicsOverlay

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
                    color: "blue"
                    width: 3

                    // create swatch image for the legend
                    Component.onCompleted: {
                        createSwatch();
                    }
                    onSwatchImageChanged: {
                        attachmentImage.source = swatchImage;
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
                    width: 3

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
                id: boundingBoxValue
                label: "Connectivity"
                SimpleLineSymbol {
                    id: boundingBoxSymbol
                    style: Enums.SimpleLineSymbolStyleDot
                    color: "yellow"
                    width: 3

                    // create swatch image for the legend
                    Component.onCompleted: {
                        createSwatch();
                    }
                    onSwatchImageChanged: {
                        boundingBoxImage.source = swatchImage;
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        visible: containerViewIsVisible
        color: "transparent"

        // Prevent MapView interaction
        MouseArea {
            anchors.fill: parent
        }
        ScrollView {
            anchors.fill: parent
        }

        Control {
            id: legendBox
            anchors {
                top: parent.top
                left: parent.left
                margins: 20
            }
            background: Rectangle {
                border.color: "black"
                border.width: 1
            }
            padding: 5

            contentItem: GridLayout {
                id: grid
                columns: 2
                anchors.horizontalCenter: parent.horizontalCenter

                Label {
                    text: "Utility association types"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }

                Image {
                    id: attachmentImage
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    id: attachmentLabel
                    text: "Attachment symbol"
                }

                Image {
                    id: connectivityImage
                }
                Label {
                    id: connectivityLabel
                    text: "Connectivity symbol"
                }

                Image {
                    id: boundingBoxImage
                }
                Label {
                    id: boundingBoxLabel
                    text: "Connectivity symbol"
                }
            }
        }

        Button {
            id: containerCloseButton
            anchors {
                bottom: parent.bottom
                bottomMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
            background: Rectangle {
                color: "white"
                border.color: "black"
            }
            text: "Close container view"
            font.pointSize: 16

            onClicked: {
                setShowContainerView(false);
                containerGraphicsOverlay.graphics.clear();
            }
        }
    }

    Control {
        id: messageBoxPopup
        anchors {
            centerIn: parent
        }
        padding: 10
        width: Math.max(messageBoxText.width, closeMessage.width) + (padding * 2)
        height: messageBoxText.height + closeMessage.height + (messageBoxPopup.padding * 3)
        background: Rectangle {
            color: "white"
            border.color: "black"
        }

        visible: messageBoxText.text !== ""

        Text {
            id: messageBoxText
            anchors {
                top: parent.top
                topMargin: messageBoxPopup.padding
                horizontalCenter: parent.horizontalCenter
            }
            text: ""
        }

        Button {
            id: closeMessage
            anchors {
                bottom: parent.bottom
                bottomMargin: messageBoxPopup.padding
                horizontalCenter: parent.horizontalCenter
            }
            text: "Close"
            onClicked: messageBoxText.text = "";
        }
    }

    function getContainerElementFromIdentifiedFeature(identifyLayersResults) {
        // Identify a feature and create a UtilityElement from it.
        containerElement = null;

        for (let i = 0; i < mapView.identifyLayersResults.length; i++) {
            let layerResult = mapView.identifyLayersResults[i];

            if (layerResult.layerContent.objectType !== "SubtypeFeatureLayer")
                continue;

            for (let j = 0; j < layerResult.sublayerResults.length; j++) {
                let sublayerResult = layerResult.sublayerResults[j];

                for (let n = 0; n < sublayerResult.geoElements.length; n++) {
                    let geoElement = sublayerResult.geoElements[n];
                    containerElement = utilityNetwork.createElementWithArcGISFeature(geoElement);

                    if (containerElement) {
                        // Queries for a list of all UtilityAssociation objects of containment association types present in the geodatabase for the containerElement.
                        utilityNetwork.associations(containerElement, Enums.UtilityAssociationTypeContainment);
                        return;
                    }
                }
            }
        }
    }

    function setShowContainerView(showContainterView) {
        containerViewIsVisible = showContainterView;
        if (containerViewIsVisible) {
            containerCloseButton.focus = true;
            previousViewpoint = mapView.currentViewpointExtent;
            mapView.map.operationalLayers.forEach((layer) => {
                                                      layer.visible = false;
                                                  });

        } else {
            mapView.focus = true;
            mapView.setViewpointAndSeconds(previousViewpoint, 0.5);
            containerGraphicsOverlay.graphics.clear();
            mapView.map.operationalLayers.forEach((layer) => {
                                                      layer.visible = true;
                                                  });
        }
    }

    function getContainmentAssociations(containmentAssociations) {
        // Create a list of elements representing the participants in the containment associations
        const contentElements = [];
        for (let i = 0; i < containmentAssociations.length; i++) {
            let otherElement = containmentAssociations[i].fromElement.objectId === containerElement.objectId ? containmentAssociations[i].toElement : containmentAssociations[i].fromElement;
            contentElements.push(otherElement);
        }
        if (contentElements.length > 0) {
            // Get the features for the UtilityElements
            utilityNetwork.featuresForElements(contentElements);
        }
    }

    function getFeaturesForElements(featuresForElementsResult) {
        // Display the features on the graphics overlay
        featuresForElementsResult.forEach((feature) => {
                                              const featureSymbol = feature.featureTable.layerInfo.drawingInfo.renderer.symbolForFeature(feature);
                                              const featureGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                                                               geometry: feature.geometry,
                                                                                                               symbol: featureSymbol
                                                                                                           });
                                              containerGraphicsOverlay.graphics.append(featureGraphic);
                                          });
        if (containerGraphicsOverlay.graphics.count > 0) {
            setShowContainerView(true);
            // Get the associations for each feature within the graphics overlay extent
            utilityNetwork.associationsWithEnvelope(containerGraphicsOverlay.extent);
        }
    }

    function showAttachmentAndConnectivitySymbols(containmentAssociations) {
        // Display the association lines on the graphics overlay
        for (let i = 0; i < containmentAssociations.length; i++) {
            let association = containmentAssociations[i]
            let symbol;

            if (association.associationType === Enums.UtilityAssociationTypeAttachment)
                symbol = attachmentSymbol
            else
                symbol = connectivitySymbol

            let graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {
                                                                    geometry: association.geometry,
                                                                    symbol: symbol
                                                                });
            containerGraphicsOverlay.graphics.append(graphic);
        }

        // If there are no associations, create a bounding box graphic using the viewpoint, otherwise use the extent of the graphics overlay
        if (containerGraphicsOverlay.graphics.count === 1 && containerGraphicsOverlay.graphics.get(0).geometry.geometryType === Enums.GeometryTypePoint) {
            let viewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter", {
                                                                      center: containerGraphicsOverlay.graphics.get(0).geometry,
                                                                      targetScale: containerElement.assetType.containerViewScale
                                                                  });
            createBoundingBoxGeometry = true;
            mapView.setViewpoint(viewpoint);
            messageBoxText.text = "This feature contains no associations";
        } else {
            boundingBoxExtent = GeometryEngine.buffer(containerGraphicsOverlay.extent, 0.05);
            containerGraphicsOverlay.graphics.append(ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: boundingBoxExtent, symbol: boundingBoxSymbol}));
            mapView.setViewpointGeometryAndPadding(containerGraphicsOverlay.extent, 80);
        }
    }
}
