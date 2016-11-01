
// Copyright 2016 ESRI
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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

SearchForWebmapSample {
    width: 800
    height: 600

    clip: true

    property real scaleFactor: System.displayScaleFactor
    property string selItem

    Component {
        id: webmapDelegate
        Rectangle {
            anchors.margins: 25
            width: webmapsList.width
            height: 24 * scaleFactor
            border.color: "white"
            border.width: 2 * scaleFactor
            color: index === webmapsList.currentIndex ? "orange" : "lightgrey"
            radius: 10

            Text {
                anchors{fill: parent; margins: 10}
                text: model.title
                color: "white"
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }

            MouseArea {
                anchors.fill: parent
                onClicked: webmapsList.currentIndex = index;
                onDoubleClicked: loadSelectedWebmap(index);
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: !portalLoaded
    }

    Rectangle {
        id: resultsBox
        visible: webmaps && !mapView.visible
        anchors {top: searchBox.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
        border.color: "grey"
        border.width: 2
        radius: 5

        Text {
            id: resultsTitle
            anchors { margins: 10; top: parent.top; left: parent.left; right: parent.right }
            text: "web maps: " + keyWordField.text
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            font.bold: true
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
        }

        ListView {
            id: webmapsList
            anchors { margins: 20; top: resultsTitle.bottom; bottom: moreResultsButton.top; left: parent.left; right: parent.right }
            clip: true
            delegate: webmapDelegate
            highlightFollowsCurrentItem: true
            model: webmaps
        }

        Button {
            id: moreResultsButton
            height: 20 * scaleFactor
            anchors { margins: 20; bottom: parent.bottom; horizontalCenter: resultsBox.horizontalCenter }
            text: "More Results"
            visible: hasMoreResults
            onClicked: searchNext();
        }
    }

    Column {
        visible: portalLoaded
        id: searchBox
        anchors {top: parent.top; horizontalCenter: parent.horizontalCenter; margins: 10 * scaleFactor}
        spacing:5

        Text {
            id: instruction
            text: qsTr("search for webmaps:")
            font.bold: true
        }

        Row {
            spacing: 5
            TextField {
                id: keyWordField
                placeholderText: "enter keyword"

                Keys.onReturnPressed: {
                    if (text.length > 0)
                        search(text);
                }
            }

            Image {
                source: "qrc:/Samples/CloudAndPortal/SearchForWebmap/searchIcon.png"
                width: height
                height: keyWordField.height

                MouseArea {
                    anchors.fill: parent
                    enabled: keyWordField.text.length > 0
                    onClicked : search(keyWordField.text);
                }
            }

            SequentialAnimation on x {
                id: noResultsAnimation
                loops: 10
                running: false
                PropertyAnimation { to: 50; duration: 20 }
                PropertyAnimation { to: 0; duration: 20 }
            }

        }
    }

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: mapView
        objectName: "mapView"
        visible: false
        anchors {top: searchBox.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
    }

    AuthenticationView {
        authenticationManager: authManager
    }

    MessageDialog {
        id: webMapMsg
        title: "Could not load web map!"
        visible: mapLoadError.length > 0
        text: mapLoadError
        onAccepted: errorAccepted();
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
