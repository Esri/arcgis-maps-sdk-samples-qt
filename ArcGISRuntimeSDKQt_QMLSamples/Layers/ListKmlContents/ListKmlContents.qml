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
    property string labelText: ""

    SceneView {
        id: sceneView
        anchors.fill: parent

        Rectangle {
            id: listViewWindow
            visible: true
            width: 200
            height: 200
//            height: childrenRect.height
            color: "lightgrey"

            ColumnLayout {

                RowLayout {
                    id: buttonRow
                    spacing: 10
                    width: parent.width

                    Button {
                        text: "<"
                        // enabled:
                        background: Rectangle {
                            color: listViewWindow.color
                        }
                        onClicked: {

                        }
                        highlighted: pressed
                    }

                    Text {
                        id: textLabel
                        text: labelText
                        wrapMode: Text.Wrap
                    }
                }

                RowLayout {
                    ListView {
                        id: myListView
//                        anchors.horizontalCenter: parent.horizontalCenter
                        height: contentHeight
                        width: 200
                        spacing: 0
                        model: nodesOnLevel
                        delegate: Component {
                            Button {
                                text: modelData
                                width: listViewWindow.width
                                onClicked: {
                                    sceneView.nodeSelected(text);
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

                    function buildTree(parentNode) {
                        let childNodes = parentNode.childNodesListModel;
                        if (childNodes !== undefined) {
                            for (let i = 0; i < childNodes.count; i++) {
                                let node = childNodes.get(i);
                                node.visible = true;
                                console.log(node.name);
                                kmlNodesList.push(node);

                                buildTree(node);
                            }
                        }
                    }

                    function displayChildren(parentNode) {
                        // if node has children, then display children
                        let childNodes = parentNode.childNodesListModel
                        if (childNodes !== undefined) {
                            // add ">" to indicate there are children
                            labelText = labelText.concat(">");

                            // clear previous node names
                            nodesOnLevel = [];
                            for (let i = 0; i < childNodes.count; i++) {
                                nodesOnLevel.push(childNodes.get(i).name);
                            }
                            myListView.model = nodesOnLevel;
                        }
                    }


                }
            }
        }
        function nodeSelected(nodeName) {
            // find node with matching name
            for (let i = 0; i < kmlNodesList.length; i++) {
                if (nodeName === kmlNodesList[i].name) {
                    let node = kmlNodesList[i];
                    labelText = labelText.concat(nodeName);


                    // set the viewpoint to the extent of the selected node
                    let nodeExtent = node.extent;
                    if (!nodeExtent.empty) {
                        sceneView.setViewpoint(ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: nodeExtent}))
                    }

                    // show the children of the node
                    kmlDataset.displayChildren(node);

                    /*
      // update current node
      m_currentNode = node;
      m_isTopLevel = false;
      emit isTopLevelChanged();
*/
                    break;
                }
            }

        }
    }
}
