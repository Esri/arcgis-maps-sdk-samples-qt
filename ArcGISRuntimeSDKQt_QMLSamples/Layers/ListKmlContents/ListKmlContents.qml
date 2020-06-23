// [WriteFile Name=ListKmlContents, Category=Layers]
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
import QtQuick.Layouts 1.11
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/kml/"
    property var nodesOnLevel: []
    property var kmlNodesList: []
    property var currentNode: null
    property var currentViewpoint: null
    property var currentCamera: null
    property string labelText: ""
    property bool topLevel: true
    property bool needsAltitudeFixed

    // recursively build list of nodes
    function buildTree(parentNode) {
        let childNodes = parentNode.childNodesListModel;
        if (childNodes !== undefined && childNodes !== null) {
            for (let i = 0; i < childNodes.count; i++) {
                let node = childNodes.get(i);
                node.visible = true;
                kmlNodesList.push(node);

                buildTree(node);
            }
        }
    }

    // recursively build string to indicate node's ancestors
    function buildPathLabel(node) {
        if (node.parentNode !== undefined && node.parentNode !== null) {
            buildPathLabel(node.parentNode);
            labelText = labelText.concat(">");
        }
        labelText = labelText.concat(node.name);
    }

    function displayChildren(parentNode) {
        // if node has children, then display children
        if (parentNode.childNodesListModel !== null && parentNode.childNodesListModel !== undefined) {
            let childNodes = parentNode.childNodesListModel
            let lastLevel = true;

            // clear previous node names
            nodesOnLevel = [];
            for (let i = 0; i < childNodes.count; i++) {
                let node = childNodes.get(i);
                nodesOnLevel.push(node.name + getKmlNodeType(node));

                // check if on last level of nodes
                if (node.childNodesListModel !== undefined && node.childNodesListModel !== null) {
                    nodesOnLevel[i] = nodesOnLevel[i].concat(" >"); // indicate there are children
                    lastLevel = false;
                }
            }
            myListView.model = nodesOnLevel;

            if (lastLevel) {
                currentNode = childNodes.get(0);
            }
        }
    }

    // display selected node on sceneview and show its children
    function processSelectedNode(nodeName) {
        // extract the node name from string, formatted "name - nodeType"
        let ind = nodeName.lastIndexOf(" - ");
        if (ind > -1) {
            nodeName = nodeName.substring(0, ind);
        }

        // find node with matching name
        for (let i = 0; i < kmlNodesList.length; i++) {
            if (nodeName === kmlNodesList[i].name) {
                topLevel = false;
                let node = kmlNodesList[i];
                currentNode = node;

                // set the viewpoint to the extent of the selected node
                getViewpointFromKmlViewpoint(currentNode);
                if (needsAltitudeFixed) {
                    getAltitudeAdjustedViewpoint(currentNode);
                } else {
                    if (currentViewpoint !== null && currentViewpoint !== undefined) {
                        sceneView.setViewpoint(currentViewpoint);
                    }
                }

                // update path label
                labelText = "";
                buildPathLabel(node);

                // show the children of the node
                displayChildren(node);
                break;
            }
        }
    }

    function getViewpointFromKmlViewpoint(node) {
        const kmlViewpoint = node.viewpoint;

        if (kmlViewpoint !== undefined && kmlViewpoint !== null) {
            // altitude adjustment is needed for all but Absolute altitude mode
            needsAltitudeFixed = (kmlViewpoint.altitudeMode !== Enums.KmlAltitudeModeAbsolute);

            switch (kmlViewpoint.type) {
            case Enums.KmlViewpointTypeLookAt:
                currentCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                          location: kmlViewpoint.location,
                                                                          distance: kmlViewpoint.range,
                                                                          heading: kmlViewpoint.heading,
                                                                          pitch: kmlViewpoint.pitch,
                                                                          roll: kmlViewpoint.roll
                                                                      });
                currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter", {center: kmlViewpoint.location,
                                                                         camera: currentCamera});
                return;
            case Enums.KmlViewpointTypeCamera:
                currentCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                          location: kmlViewpoint.location,
                                                                          heading: kmlViewpoint.heading,
                                                                          pitch: kmlViewpoint.pitch,
                                                                          roll: kmlViewpoint.roll
                                                                      });
                currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter", {center: kmlViewpoint.location,
                                                                         camera: currentCamera});
                return;
            default:
                console.warn("Unexpected KmlViewpointType");
                return;
            }
        }

        // if viewpoint is empty then use node's extent
        const nodeExtent = node.extent;
        if (nodeExtent !== null && nodeExtent !== undefined && !nodeExtent.empty) {
            // when no altitude is specified, assume elevation needs to be adjusted
            needsAltitudeFixed = true;

            if (nodeExtent.width === 0 && nodeExtent.height === 0) {
                currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: nodeExtent});
                currentCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                          location: nodeExtent.center,
                                                                          distance: 1000,
                                                                          heading: 0,
                                                                          pitch: 45,
                                                                          roll: 0
                                                                      });
                sceneView.setViewpointCameraAndWait(currentCamera);
                return;
            } else {
                // add padding to extent
                const bufferDistance = Math.max(nodeExtent.width, nodeExtent.height) / 20;
                const spatialReferenceWgs84 = ArcGISRuntimeEnvironment.createObject("SpatialReference", {wkid: 4326});
                const bufferedExtent = ArcGISRuntimeEnvironment.createObject("Envelope", {
                                                                                 xMin: nodeExtent.xMin - bufferDistance,
                                                                                 yMin: nodeExtent.yMin - bufferDistance,
                                                                                 xMax: nodeExtent.xMax + bufferDistance,
                                                                                 yMax: nodeExtent.yMax + bufferDistance,
                                                                                 zMin: nodeExtent.zMin - bufferDistance,
                                                                                 zMax: nodeExtent.zMax + bufferDistance,
                                                                                 spatialReference: spatialReferenceWgs84
                                                                             });
                currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: bufferedExtent});
            }
        } else {
            // can't show viewpoint
            needsAltitudeFixed = false;
            currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter");
        }
    }

    function getAltitudeAdjustedViewpoint(node) {
        // assume altitude mode is clamp-to-ground if not specified
        let altMode = Enums.KmlAltitudeModeClampToGround;
        let kmlViewpoint = node.viewpoint;

        if (kmlViewpoint !== undefined && kmlViewpoint !== null) {
            altMode = kmlViewpoint.altitudeMode;
        }

        // if altitude mode is Absolute, viewpoint doesn't need adjustment
        if (altMode === Enums.KmlAltitudeModeAbsolute) {
            return;
        }

        if (currentViewpoint.viewpointType === Enums.ViewpointTypeBoundingGeometry) {
            scene.baseSurface.locationToElevation(currentViewpoint.extent.center);
        }
        else if (currentViewpoint.viewpointType === Enums.ViewpointTypeCenterAndScale) {
            scene.baseSurface.locationToElevation(currentViewpoint.center);
        }
    }

    // returns string containing the KmlNodeType
    function getKmlNodeType(node) {
        let type = "";
        switch(node.kmlNodeType) {
        case Enums.KmlNodeTypeKmlDocument:
            type = "KmlDocument";
            break;
        case Enums.KmlNodeTypeKmlFolder:
            type = "KmlFolder";
            break;
        case Enums.KmlNodeTypeKmlGroundOverlay:
            type = "KmlGroundOverlay";
            break;
        case Enums.KmlNodeTypeKmlNetworkLink:
            type = "KmlNetworkLink";
            break;
        case Enums.KmlNodeTypeKmlPhotoOverlay:
            type = "KmlPhotoOverlay";
            break;
        case Enums.KmlNodeTypeKmlPlacemark:
            type = "KmlPlacemark";
            break;
        case Enums.KmlNodeTypeKmlScreenOverlay:
            type = "KmlScreenOverlay";
            break;
        case Enums.KmlNodeTypeKmlTour:
            type = "KmlTour";
            break;
        }
        return " - " + type;
    }

    SceneView {
        id: sceneView
        anchors.fill: parent

        // create window for displaying the KML contents
        Control {
            width: 300
            background : Rectangle {
                color: "lightgrey"
            }
            contentItem: GridLayout {
                columns: 2

                Button {
                    id: backButton
                    Layout.margins: 3
                    text: "<"
                    enabled: !topLevel
                    flat: true
                    highlighted: pressed
                    onClicked: {
                        // display previous level of nodes
                        let parentNode = currentNode.parentNode;
                        let grandparentNode = parentNode.parentNode;

                        if (grandparentNode !== undefined && grandparentNode !== null) {
                            labelText = "";
                            buildPathLabel(grandparentNode);

                            displayChildren(grandparentNode);
                            currentNode = grandparentNode;
                        }
                        // if parent node is undefined, then at top of tree
                        else {
                            labelText = "";
                            buildPathLabel(parentNode);

                            displayChildren(parentNode);
                            topLevel = true;
                        }

                        if (currentNode.name === "") {
                            topLevel = true;
                        }
                    }
                }

                Text {
                    Layout.fillWidth: true
                    id: textLabel
                    text: labelText
                    wrapMode: Text.Wrap
                }

                ListView {
                    id: myListView
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    model: nodesOnLevel
                    Layout.preferredHeight: contentHeight
                    delegate: Component {
                        Button {
                            text: modelData
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                            onClicked: {
                                processSelectedNode(text);
                            }
                            highlighted: pressed
                        }
                    }
                }
            }
        }

        Scene {
            id: scene
            BasemapImageryWithLabels {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }

                onLocationToElevationStatusChanged: {
                    if (locationToElevationStatus !== Enums.TaskStatusCompleted)
                        return;

                    // assume altitude mode is clamp-to-ground if not specified
                    let altMode = Enums.KmlAltitudeModeClampToGround;
                    let kmlViewpoint = currentNode.viewpoint;

                    if (kmlViewpoint !== undefined && kmlViewpoint !== null) {
                        altMode = kmlViewpoint.altitudeMode;
                    }

                    // if altitude mode is Absolute, viewpoint doesn't need adjustment
                    if (altMode === Enums.KmlAltitudeModeAbsolute)
                        return;

                    if (currentViewpoint.viewpointType === Enums.ViewpointTypeBoundingGeometry) {
                        let lookAtExtent = currentViewpoint.extent;
                        let target = null;
                        if (altMode === Enums.KmlAltitudeModeClampToGround) {
                            // if depth of extent = 0, add 100m to the elevation to get zMax
                            if (lookAtExtent.depth === 0) {
                                target = ArcGISRuntimeEnvironment.createObject("Envelope",{
                                                                                       xMin: lookAtExtent.xMin,
                                                                                       yMin: lookAtExtent.yMin,
                                                                                       xMax: lookAtExtent.xMax,
                                                                                       yMax: lookAtExtent.yMax,
                                                                                       zMin: locationToElevationResult,
                                                                                       zMax: locationToElevationResult + 100,
                                                                                       spatialReference: lookAtExtent.spatialReference
                                                                                   });
                            } else {
                                target = ArcGISRuntimeEnvironment.createObject("Envelope",{
                                                                                       xMin: lookAtExtent.xMin,
                                                                                       yMin: lookAtExtent.yMin,
                                                                                       xMax: lookAtExtent.xMax,
                                                                                       yMax: lookAtExtent.yMax,
                                                                                       zMin: locationToElevationResult,
                                                                                       zMax: lookAtExtent.depth + locationToElevationResult,
                                                                                       spatialReference: lookAtExtent.spatialReference
                                                                                   });
                            }
                        } else {
                            target = ArcGISRuntimeEnvironment.createObject("Envelope",{
                                                                                   xMin: lookAtExtent.xMin,
                                                                                   yMin: lookAtExtent.yMin,
                                                                                   xMax: lookAtExtent.xMax,
                                                                                   yMax: lookAtExtent.yMax,
                                                                                   zMin: lookAtExtent.zMin + locationToElevationResult,
                                                                                   zMax: lookAtExtent.zMax + locationToElevationResult,
                                                                                   spatialReference: lookAtExtent.spatialReference
                                                                               });
                        }

                        // if node has viewpoint specified, return adjusted geometry with adjusted camera
                        if (kmlViewpoint !== undefined && kmlViewpoint !== null) {
                            sceneView.setViewpointCameraAndWait(currentViewpoint.camera.elevate(locationToElevationResult));
                            currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: target});
                            sceneView.setViewpoint(currentViewpoint);
                            return;
                        } else {
                            currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: target});
                            sceneView.setViewpoint(currentViewpoint);
                            return;
                        }
                    } else if (currentViewpoint.viewpointType === Enums.ViewpointTypeCenterAndScale) {
                        let targetPoint = null;
                        let lookAtPoint = currentViewpoint.center;
                        if (altMode === Enums.KmlAltitudeModeClampToGround) {
                            targetPoint = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                                    x: lookAtPoint.x, y: lookAtPoint.y,
                                                                                    z: locationToElevationResult,
                                                                                    spatialReference: lookAtPoint.spatialReference
                                                                                });
                        } else {
                            targetPoint = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                                    x: lookAtPoint.x, y: lookAtPoint.y,
                                                                                    z: lookAtPoint.z + locationToElevationResult,
                                                                                    spatialReference: lookAtPoint.spatialReference
                                                                                });
                        }

                        // set the viewpoint using the adjusted target
                        if (kmlViewpoint !== undefined && kmlViewpoint !== null) {
                            sceneView.setViewpointCameraAndWait(currentViewpoint.camera.elevate(locationToElevationResult));
                            currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter", {center: targetPoint});
                            return;
                        } else {
                            // use Google Earth default values to set camera
                            currentViewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointCenter", {center: targetPoint});
                            sceneView.setViewpoint(currentViewpoint);
                            currentCamera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                                      location: targetPoint,
                                                                                      distance: 1000,
                                                                                      heading: 0,
                                                                                      pitch: 45,
                                                                                      roll: 0
                                                                                  });
                            sceneView.setViewpointCameraAndWait(currentCamera);
                            return;
                        }
                    }
                }
            }

            KmlLayer {
                KmlDataset {
                    id: kmlDataset
                    url: dataPath + "esri_test_data.kmz"

                    onLoadStatusChanged: {
                        if (loadStatus !== Enums.LoadStatusLoaded)
                            return;

                        for (let i = 0; i < rootNodes.length; i++) {
                            nodesOnLevel.push(rootNodes[i].name);
                            kmlNodesList.push(rootNodes[i]);

                            buildTree(rootNodes[i]);
                        }

                        // if at top node, then display children
                        if (kmlNodesList.length !== 0 && kmlNodesList[0].parentNode === null) {
                            displayChildren(kmlNodesList[0]);
                        }
                    }
                }
            }
        }
    }
}
