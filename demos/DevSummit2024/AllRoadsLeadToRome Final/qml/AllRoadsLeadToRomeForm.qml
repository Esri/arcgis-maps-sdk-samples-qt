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
        width: showDirections ? parent.width * (2/3) : parent.width
        // set focus to enable keyboard navigation
        focus: true
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    AllRoadsLeadToRome {
        id: appBackend
        mapView: view
    }

    // Create a BusyIndicator
    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: appBackend.busy
    }

    Button {
        id: showDirsBtn
        anchors {
            top: parent.top
            right: parent.right
        }

        visible: !appBackend.busy && appBackend.routeTime > 0
        text: showDirections ? "Hide directions" : "Show directions"
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
        width: parent.width / 3
        visible: showDirections && !appBackend.busy
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
            wrapMode: Text.WordWrap

            text: "Destination: " + appBackend.destinationName +
                  "\nTotal time: " + Math.floor(appBackend.routeTime/60) + " hours " + (Math.floor(appBackend.routeTime % 60)) + " mins" +
                  "\nTotal distance: " + (appBackend.routeLength/1000).toFixed(1) + " km"
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

        ScrollView {
            id: scrollView
            anchors {
                top: headerText.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 10
            }
            clip: true

            Column {
                spacing: 10

                Repeater {
                    model: appBackend.directions

                    delegate: Text {
                        property var geometry: model.geometry
                        text: model.length > 0 ? model.directionText + "\n" + (model.length/1000).toFixed(2) + " km, " + Math.floor(model.duration) + " mins" : model.directionText
                        wrapMode: Text.Wrap
                        width: scrollView.width-20

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                appBackend.goToDirection(geometry);
                            }
                        }
                    }
                }
            }
        }
    }
}
