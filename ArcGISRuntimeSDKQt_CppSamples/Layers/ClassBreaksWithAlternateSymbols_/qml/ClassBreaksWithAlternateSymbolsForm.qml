// Copyright 2022 ESRI
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

import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.ClassBreaksWithAlternateSymbols 1.0

Item {

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        spacing: 5

        Button {
            id: four_million_scale
            width: 200
            enabled: true
            text: "Set Scale to 4,000,000"
            onClicked: model.setScale(4000000);
        }

        Button {
            id: five_million_scale
            width: 200
            enabled: true
            text: "Set Scale to 5,000,000"
            onClicked: model.setScale(5000000);
        }

        Button {
            id: six_million_scale
            width: 200
            enabled: true
            text: "Set Scale to 6,000,000"
            onClicked: model.setScale(6000000);
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ClassBreaksWithAlternateSymbols {
        id: model
        mapView: view
    }
}
