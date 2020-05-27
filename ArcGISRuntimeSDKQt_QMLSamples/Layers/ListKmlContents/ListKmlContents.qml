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
    property var currentNodesList
    property var currentNode: null
    property string labelText: ""
    property bool topLevel: true
    property bool selectedLastLevel: false
    //    property bool lastLevel: false

    function removeLabelLayer(label) {
        let ind = label.lastIndexOf(">");
        if (ind === label.length - 1) {
            label = label.slice(0, -1);
            ind = label.lastIndexOf(">");
        }
        label = label.substring(0, ind);
        return label;
    }

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

    function displayChildren(parentNode) {
        // if node has children, then display children
        if (parentNode.childNodesListModel !== null && parentNode.childNodesListModel !== undefined) {
            let childNodes = parentNode.childNodesListModel
            let lastLevel = true;

            // add ">" to indicate there are children
            labelText = labelText.concat(">");

            // clear previous node names
            nodesOnLevel = [];
            for (let i = 0; i < childNodes.count; i++) {
                nodesOnLevel.push(childNodes.get(i).name);

                // check if on last level of nodes
                if (childNodes.get(i).childNodesListModel !== undefined && childNodes.get(i).childNodesListModel !== null) {
                    lastLevel = false;
                }
            }
            myListView.model = nodesOnLevel;
            if (lastLevel) {
                currentNode = childNodes.get(0);
            }
        }
    }

    function nodeSelected(nodeName) {
        // find node with matching name
        for (let i = 0; i < kmlNodesList.length; i++) {
            if (nodeName === kmlNodesList[i].name) {
                topLevel = false;
                let node = kmlNodesList[i];
                currentNode = node;
                labelText = labelText.concat(currentNode.name);

                // set the viewpoint to the extent of the selected node
                let nodeExtent = node.extent;
                if (!nodeExtent.empty) {
                    sceneView.setViewpoint(ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: nodeExtent}))
                }

                selectedLastLevel = (node.childNodesListModel === null || node.childNodesListModel === undefined);

                // show the children of the node
                displayChildren(node);
                break;
            }
        }
    }

    SceneView {
        id: sceneView
        anchors.fill: parent

        Rectangle {
            id: listViewWindow
            visible: true
            width: 200
            height: childrenRect.height
            color: "lightgrey"

            ColumnLayout {

                RowLayout {
                    id: buttonRow
                    spacing: 10
                    width: parent.width

                    Button {
                        id: backButton
                        text: "<"
                        enabled: !topLevel
                        background: Rectangle {
                            color: listViewWindow.color
                        }
                        onClicked: {
                            // display previous level of nodes
                            let parentNode = currentNode.parentNode;
                            let grandparentNode = parentNode.parentNode;

                            // remove last node's name from label
                            labelText = removeLabelLayer(labelText);

                            if (selectedLastLevel) {
                                labelText = removeLabelLayer(labelText);
                            }

                            if (grandparentNode !== undefined && grandparentNode !== null) {
                                displayChildren(grandparentNode);
                                currentNode = grandparentNode;
                            }
                            // if parent node is undefined, then at top of tree
                            else {
                                displayChildren(parentNode);
                                topLevel = true;
                            }

                            if (currentNode.name === "") {
                                topLevel = true;
                            }
                        }

                        highlighted: pressed
                    }

                    Rectangle {
                        id: textRectangle
                        width: listViewWindow.width - backButton.width - buttonRow.spacing
                        height: backButton.height
                        color: listViewWindow.color
                        Text {
                            id: textLabel
                            text: labelText
                            width: textRectangle.width
                            wrapMode: Text.Wrap
                        }
                    }
                }

                RowLayout {
                    ListView {
                        id: myListView
                        // anchors.horizontalCenter: parent.horizontalCenter
                        height: contentHeight
                        width: 200
                        spacing: 0
                        model: nodesOnLevel
                        delegate: Component {
                            Button {
                                text: modelData
                                width: listViewWindow.width
                                onClicked: {
                                    nodeSelected(text);
                                }
                                highlighted: pressed
                            }
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
