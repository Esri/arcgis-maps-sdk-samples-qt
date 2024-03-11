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
import Esri.GraphicsMemoryManagement

Item {

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    GraphicsMemoryManagement {
        id: gmmDemo
        mapView: view
    }

    Rectangle {
        color: "white"
        border.color: "black"
        x: 5
        y: 5
        width: col.width + 15
        height: col.height + 15

        Column {
            id: col
            anchors.centerIn: parent
            spacing: 10

            Button {
                id: button
                width: parent.width
                Text {
                    anchors.centerIn: parent
                    text: "Add graphics with parent: 'this'"
                    wrapMode: Text.WordWrap
                }
                onClicked: gmmDemo.badMemoryManagement();
            }

            Button {
                width: parent.width
                Text {
                    anchors.centerIn: parent
                    text: "Add graphics with parent: scoped QObject"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    width: parent.width
                }
                onClicked: gmmDemo.properMemoryManagement();
            }

            Button {
                width: parent.width
                Text {
                    anchors.centerIn: parent
                    text: "Clear graphics"
                }
                onClicked: gmmDemo.clearGraphics();
            }

            Text {
                text: "Current graphics count: " + gmmDemo.graphicsOverlayCount;
            }
        }
    }
}
