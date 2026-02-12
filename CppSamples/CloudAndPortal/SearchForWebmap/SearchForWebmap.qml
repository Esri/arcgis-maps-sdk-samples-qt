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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

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

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Component {
        id: webmapDelegate

        Rectangle {
            anchors.margins: 25
            width: webmapsList.width
            height: 32
            border.color: "white"
            border.width: 2
            color: index === webmapsList.currentIndex ? palette.highlight : palette.mid
            radius: 10

            //! [PortalItemListModel example QML delegate]
            Label {
                anchors {
                    fill: parent
                    margins: 10
                }
                text: title // access the title role of the model
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
        visible: !portalLoaded
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
        color: palette.base

        Label {
            id: resultsTitle
            anchors {
                margins: 10
                top: parent.top
                left: parent.left
                right: parent.right
            }
            text: qsTr("Web maps: ") + keyWordField.text
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
            text: qsTr("More Results")
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

        Label {
            id: instruction
            text: qsTr("Search for webmaps:")
            font.bold: true
        }

        Row {
            spacing: 5

            TextField {
                id: keyWordField
                placeholderText: qsTr("Enter keyword")
                selectByMouse: true

                Keys.onReturnPressed: {
                    if (text.length > 0)
                        search(text);
                }
            }

            ToolButton {
                width: height
                height: keyWordField.height
                enabled: keyWordField.text.length > 0
                flat: true
                icon {
                    source: "qrc:/Samples/CloudAndPortal/SearchForWebmap/search-24.svg"
                    width: 24
                    height: 24
                    color: palette.text
                }
                onClicked: search(keyWordField.text)

                background: Rectangle {
                    color: "transparent"
                    radius: 4
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

    // Declare Authenticator to handle any authentication challenges
    Authenticator {
        anchors.fill: parent
    }

    Dialog {
        id: webMapMsg
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: qsTr("Could not load web map!")
        visible: mapLoadError.length > 0
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Label {
                id: textLabel
                text: mapLoadError
            }
            Label {
                id: detailsLabel
            }
        }
        onAccepted: errorAccepted()
    }
}
