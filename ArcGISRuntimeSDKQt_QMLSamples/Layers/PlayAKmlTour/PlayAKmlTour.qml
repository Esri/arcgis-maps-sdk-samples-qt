// [WriteFile Name=PlayAKmlTour, Category=Layers]
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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data"

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            KmlLayer {
               dataset: KmlDataset {
                   id: kmlDataset
                   url: dataPath + "/kml/Esri_tour.kmz"
               }
               onLoadStatusChanged: {
                   if (loadStatus !== Enums.LoadStatusLoaded)
                       return;

                   const kmlTour = findFirstKMLTour(kmlDataset.rootNodes)

                   if (kmlTour !== null) {
                       kmlTour.tourStatusChanged.connect(()=> {
                           switch(kmlTour.tourStatus) {
                           case Enums.KmlTourStatusCompleted:
                           case Enums.KmlTourStatusInitialized:
                               playButton.enabled = true;
                               pauseButton.enabled = false;
                               resetButton.enabled = true;
                               break;
                           case Enums.KmlTourStatusPaused:
                               playButton.enabled = true;
                               pauseButton.enabled = false;
                               resetButton.enabled = true;
                               break;
                           case Enums.KmlTourStatusPlaying:
                               playButton.enabled = false;
                               pauseButton.enabled = true;
                               resetButton.enabled = true;
                               break;
                           case Enums.KmlTourStatusInitializing:
                           case Enums.KmlTourStatusNotInitialized:
                               break;
                           }
                       });
                       kmlTourController.tour = kmlTour;
                   }
               }
            }

            KmlTourController {
                id: kmlTourController
            }
        }

        Rectangle {
            id: buttonBackground
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#000000"
            opacity: .75
            radius: 5

            // catch mouse signals from propagating to parent
            MouseArea {
                anchors.fill: parent
                onClicked: mouse.accepted = true
                onWheel: wheel.accepted = true
            }

            RowLayout {
                spacing: 0
                Button {
                    id: playButton
                    text: qsTr("Play")
                    Layout.margins: 2
                    enabled: false
                    onClicked: kmlTourController.play();
                }

                Button {
                    id: pauseButton
                    text: qsTr("Pause")
                    Layout.margins: 2
                    enabled: false
                    onClicked: kmlTourController.pause();
                }

                Button {
                    id: resetButton
                    text: qsTr("Reset")
                    Layout.margins: 2
                    enabled: false
                    onClicked: kmlTourController.reset();
                }
            }
        }
    }

    function findFirstKMLTour(nodes) {
        for (let i = 0; i < nodes.length; i++) {
            const node = nodes[i];
            if (node.kmlNodeType === Enums.KmlNodeTypeKmlTour)
                return node;
            else if ((node.kmlNodeType === Enums.KmlNodeTypeKmlFolder) || (node.kmlNodeType === Enums.KmlNodeTypeKmlDocument))
                return findFirstKMLTourFromListModel(node.childNodesListModel);
        }
        return null;
    }

    function findFirstKMLTourFromListModel(nodes) {
        for (let i = 0; i < nodes.count; ++i) {
            const node = nodes.get(i);
            if (node.kmlNodeType === Enums.KmlNodeTypeKmlTour)
                return node;
            else if ((node.kmlNodeType === Enums.KmlNodeTypeKmlFolder) || (node.kmlNodeType === Enums.KmlNodeTypeKmlDocument))
                return findFirstKMLTourFromListModel(node.childNodesListModel);
        }
        return null;
    }
}
