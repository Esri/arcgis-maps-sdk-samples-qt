
// Copyright 2019 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.11

ApplicationWindow {
    id: appWindow
    width: 800
    height: 600
    title: "Display3DLabelsInScene"

    // add a sceneView component
    SceneView {
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
                                                           gasLayer.labelsEnabled  = true;

                                                       }
                                                   });
            }
        }

        TextSymbol {
            id: gasLineLabelSymbol
            angle: 0
            backgroundColor: "transparent"
            outlineColor: "white"
            color: "#ffa500"
            haloColor: "white"
            haloWidth: 2.0
            horizontalAlignment: Enums.HorizontalAlignmentCenter
            verticalAlignment: Enums.VerticalAlignmentMiddle
            kerningEnabled: false
            offsetX: 0
            offsetY: 0
            fontDecoration: Enums.FontDecorationNone
            size: 14.0
            fontStyle: Enums.FontStyleNormal
            fontWeight: Enums.FontStyleNormal
        }

        LabelDefinition {
            id: gasLineLabelDefinition
            json: {
                "labelExpressionInfo": {
                    "expression": "$feature.INSTALLATIONDATE"
                },
                "labelPlacement": "esriServerLinePlacementAboveAlong",
                "symbol": gasLineLabelSymbol.json
            }
        }
    }
}
