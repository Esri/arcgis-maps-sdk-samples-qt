
// Copyright 2021 ESRI
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
import Esri.Display3DLabelsInScene 1.0

Item {

    // Create SceneQuickView here, and create its Scene etc. in C++ code
    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    Display3DLabelsInScene {
        id: model
        sceneView: view
    }
}
