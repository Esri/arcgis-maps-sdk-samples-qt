// [WriteFile Name=ShowOrgBasemaps, Category=CloudAndPortal]
// [Legal]
// Copyright 2016 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.5

ShowOrgBasemapsSample {
    width: 800
    height: 600
    clip: true

    
    onPortalLoadedChanged: { gridFadeIn.running = true; }

    BusyIndicator {
        anchors.centerIn: parent
        running: !mapView.visible && !portalLoaded & !anonymousLogIn.visible
    }

    Text {
        id: titleLabel
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: 10
        }
        font.pointSize: 14
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
        text: anonymousLogIn.visible ? "Load Portal" :
                                       (basemapsGrid.count > 0 ? orgName + " Basemaps" : "Loading Organization Basemaps...")
        wrapMode: Text.Wrap
        elide: Text.ElideRight
    }

    GridView {
        id: basemapsGrid
        anchors {
            top: titleLabel.bottom;
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right
        }
        visible: portalLoaded
        cellWidth: 128;
        cellHeight: 128
        opacity: 0
        focus: true
        clip: true
        model: basemaps

        delegate: Rectangle {
            anchors.margins: 5
            width: basemapsGrid.cellWidth
            height: width
            border {width: 2; color: index === basemapsGrid.currentIndex ? "blue" : "lightgrey"}
            color: index === basemapsGrid.currentIndex ? "yellow" : "white"
            radius: 2
            clip: true

            //! [BasemapListModel example QML delegate]
            Image {
                id: basemapImg
                anchors {
                    bottom: basemapLabel.top;
                    horizontalCenter: parent.horizontalCenter
                }
                height: parent.height - ( basemapLabel.height * 2 );
                source: thumbnailUrl // use the thumbnailUrl role of the model
                width: height
                fillMode: Image.PreserveAspectCrop
            }

            Text {
                id: basemapLabel
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    right: parent.right
                }
                height: 16
                z: 100
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                text: title // use the title role of the model
                font.pointSize: 8
                font.bold: index === basemapsGrid.currentIndex
            }
            //! [BasemapListModel example QML delegate]

            MouseArea {
                enabled: !mapView.visible && portalLoaded
                anchors.fill: parent

                onClicked: {
                    if (!enabled)
                        return;

                    basemapsGrid.currentIndex = index;
                }
                onDoubleClicked: {
                    if (!enabled)
                        return;

                    selectedAnimation.running = true;
                    titleLabel.text = title;
                    loadSelectedBasemap(index);
                    gridFadeOut.running = true;
                }
            }

            SequentialAnimation on opacity {
                id: selectedAnimation
                running: false
                loops: 4
                PropertyAnimation { to: 0; duration: 60 }
                PropertyAnimation { to: 1; duration: 60 }
            }
        }

        OpacityAnimator on opacity {
            id: gridFadeIn
            from: 0;
            to: 1;
            duration: 2000
            running: false
        }

        OpacityAnimator on opacity {
            id: gridFadeOut
            from: 1;
            to: 0;
            duration: 2000
            running: false
        }
    }

    MapView {
        id: mapView
        objectName: "mapView"
        anchors {
            top: titleLabel.bottom;
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right
        }
        visible: false
    }

    Button {
        id: backButton
        anchors {
            top: mapView.top
            right: mapView.right
            margins: 16
        }
        visible: mapView.visible
        text: "Back"
        icon.source: "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ic_menu_back_dark.png"

        opacity: hovered ? 1 : 0.5

        onClicked: {
            titleLabel.text = "Basemaps";
            mapView.visible = false;
            basemapsGrid.enabled = true;
            gridFadeIn.running = true;
        }
    }

    Button {
        id: anonymousLogIn
        anchors {
            margins: 16
            horizontalCenter: parent.horizontalCenter
            top: titleLabel.bottom
        }
        text: "Anonymous"
        icon.source: "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ic_menu_help_dark.png"

        onClicked: {
            load(true);
            anonymousLogIn.visible = false;
            userLogIn.visible = false;
        }
    }

    Button {
        id: userLogIn
        anchors {
            margins: 16
            horizontalCenter: anonymousLogIn.horizontalCenter
            top: anonymousLogIn.bottom
        }
        width: anonymousLogIn.width
        text: "Sign-in"
        icon.source: "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ic_menu_account_dark.png"

        onClicked: {
            load(false);
            anonymousLogIn.visible = false;
            userLogIn.visible = false;
        }
    }

    /*
    // Uncomment this section when running as standalone application
    AuthenticationView {
        authenticationManager: authManager
    }
    */
}
