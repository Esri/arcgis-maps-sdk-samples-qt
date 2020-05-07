// [WriteFile Name=SearchForWebmap, Category=CloudAndPortal]
// [Legal]
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
// [Legal]

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

SearchForWebmapSample {
    id: root
    width: 800
    height: 600
    clip: true
    
    property string selItem

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: mapView
        objectName: "mapView"
        anchors {
            top: searchBox.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }
        visible: false
    }

    Component {
        id: webmapDelegate

        Rectangle {
            anchors.margins: 25
            width: webmapsList.width
            height: 32
            border.color: "white"
            border.width: 2
            color: index === webmapsList.currentIndex ? "orange" : "lightgrey"
            radius: 10

            //! [PortalItemListModel example QML delegate]
            Text {
                anchors {
                    fill: parent
                    margins: 10
                }
                text: title // access the title role of the model
                color: "white"
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }
            //! [PortalItemListModel example QML delegate]

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
        anchors {
            top: searchBox.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }
        visible: webmaps && !mapView.visible
        border.color: "grey"
        border.width: 2
        radius: 5

        Text {
            id: resultsTitle
            anchors {
                margins: 10
                top: parent.top
                left: parent.left
                right: parent.right
            }
            text: "web maps: " + keyWordField.text
            elide: Text.ElideRight
            wrapMode: Text.Wrap
            font.bold: true
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
        }

        ListView {
            id: webmapsList
            anchors {
                margins: 20
                top: resultsTitle.bottom
                bottom: moreResultsButton.top
                left: parent.left
                right: parent.right
            }
            clip: true
            delegate: webmapDelegate
            highlightFollowsCurrentItem: true
            model: webmaps
        }

        Button {
            id: moreResultsButton
            anchors {
                margins: 20
                bottom: parent.bottom
                horizontalCenter: resultsBox.horizontalCenter
            }
            visible: hasMoreResults
            text: "More Results"
            onClicked: searchNext();
        }
    }

    Column {
        id: searchBox
        anchors {
            top: parent.top;
            horizontalCenter: parent.horizontalCenter
            margins: 10
        }
        visible: portalLoaded
        spacing: 5

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
                selectByMouse: true

                Keys.onReturnPressed: {
                    if (text.length > 0)
                        search(text);
                }
            }

            Image {
                width: height
                height: keyWordField.height
                source: "qrc:/Samples/CloudAndPortal/SearchForWebmap/searchIcon.png"

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

    // Uncomment this section when running as standalone application
    /*
    AuthenticationView {
        authenticationManager: authManager
    }
    */

    Dialog {
        id: webMapMsg
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Could not load web map!"
        visible: mapLoadError.length > 0
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: mapLoadError
            }
            Text {
                id: detailsLabel
            }
        }
        onAccepted: errorAccepted()
    }
}
