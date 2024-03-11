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

    property bool showDirections: false

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: showDirections ? parent.width *.75 : parent.width
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

    Button {
        id: showDirsBtn
        anchors {
            top: parent.top
            right: parent.right
        }

        text: showDirections ? "Hide directions" : "Show directions"
        visible: !model.busy && model.routeTime > 0
        onVisibleChanged: if (!visible) showDirections = false
        onClicked: showDirections = !showDirections
    }

    Rectangle {
        id: dirRect
        anchors {
            top: showDirsBtn.bottom
            right: parent.right
            bottom: parent.bottom
        }
        anchors.leftMargin: showDirections ? 0 : parent.width / 4
        width: parent.width / 4
        visible: showDirections
        color: "white"

        Text {
            id: headerText
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
            }

            font.bold: true

            text: "Destination: " + model.destinationName +
                  "\nTotal time: " + Math.floor(model.routeTime/60) + " hours " + (Math.floor(model.routeTime % 60)) + " mins" +
                  "\nTotal distance: " + (model.routeLength/1000).toFixed(1) + " km"
        }

        Rectangle {
            anchors {
                top: headerText.bottom
                left: parent.left
                right: parent.right
                margins: 5
            }

            height: 1

            color: "black"
        }

        ListView {
            anchors {
                top: headerText.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10
            }
            clip: true

            spacing: 10

            model: model.directions

            delegate: Text {
                text: length > 0 ? directionText + "\n" + (length/1000).toFixed(2) + " km, " + Math.floor(duration) + " mins" : directionText
                wrapMode: Text.Wrap
                width: dirRect.width-20
            }
        }
    }
}
