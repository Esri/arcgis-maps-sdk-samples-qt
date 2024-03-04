// Copyright 2024 ESRI
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

import QtQuick
import QtQuick.Controls
import Esri.AllRoadsLeadToRome

Item {

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: parent.width *.75
        // set focus to enable keyboard navigation
        focus: true
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    AllRoadsLeadToRome {
        id: model
        mapView: view
    }

    // Create a BusyIndicator
    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: model.busy
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: parent.width / 4
        visible: true//model.directions.length > 0
        color: "white"
        ListView {
            anchors.fill: parent
            spacing: 10

            model: model.directions

            delegate: Text {
                text: directionText
            }
        }
    }

}
