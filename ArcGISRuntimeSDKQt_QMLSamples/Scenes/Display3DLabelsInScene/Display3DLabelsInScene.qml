// [WriteFile Name=Display3DLabelsInScene, Category=Scenes]
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

import QtQuick 2.6
import Esri.ArcGISRuntime 100.11

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        // add a Scene to the SceneView
        Scene {
            id: nycScene
            initUrl: "https://www.arcgis.com/home/item.html?id=850dfee7d30f4d9da0ebca34a533c169"

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                nycScene.operationalLayers.forEach(layer => {
                                                       if (layer.name === "Gas") {
                                                           // The Gas layer is a GroupLayer type and we must extract our target layer from it
                                                           let gasLayer = layer.layers.get(0);
                                                           gasLayer.labelDefinitions.clear();
                                                           gasLayer.labelDefinitions.append(gasLineLabelDefinition);
                                                           gasLayer.labelsEnabled = true;
                                                       }
                                                   });
            }
        }


        LabelDefinition {
            id: gasLineLabelDefinition

            TextSymbol {
                id: gasLineLabelSymbol
                color: "#ffa500"
                haloColor: "white"
                haloWidth: 2.0
                size: 16.0
            }

            ArcadeLabelExpression {
                expression: "Text($feature.INSTALLATIONDATE, 'D MMMM Y')"
            }

            placement: Enums.LabelingPlacementLineAboveAlong
            useCodedValues: true
        }
    }
}
