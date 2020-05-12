// [WriteFile Name=ShowOrgBasemapPicker, Category=CloudAndPortal]
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property var porInfo: portal.portalInfo

    function chooseBasemap(selectedBasemap) {
        title.text = selectedBasemap.item.title;
        basemapsGrid.enabled = false;

        const newMap = ArcGISRuntimeEnvironment.createObject("Map", {item: selectedBasemap.item});
        mapView.map = newMap;
        gridFadeOut.running = true;
        mapView.visible = true;
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: !anonymousLogIn.visible && !mapView.visible && portal.loadStatus !== Enums.LoadStatusLoaded;
    }

    Credential {
        id: oAuthCredential
        oAuthClientInfo: OAuthClientInfo {
            oAuthMode: Enums.OAuthModeUser
            clientId: "iLkGIj0nX8A4EJda"
        }
    }

    Portal {
        id: portal

        //! [Portal fetchBasemaps after loaded]
        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad) {
                retryLoad();
                return;
            }

            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            fetchBasemaps();
        }

        onFetchBasemapsStatusChanged: {
            if (fetchBasemapsStatus !== Enums.TaskStatusCompleted)
                return;

            basemapsGrid.model = basemaps;
            gridFadeIn.running = true;
        }
        //! [Portal fetchBasemaps after loaded]
    }

    Text{
        id: title
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
                                       (basemapsGrid.count > 0 ? porInfo.organizationName + " Basemaps" : "Loading Organization Basemaps...")
        wrapMode: Text.Wrap
        elide: Text.ElideRight
    }

    MapView {
        id: mapView
        anchors {
            top: title.bottom;
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
        icon.source: "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ic_menu_back_dark.png"
        text: "Back"
        opacity: hovered ? 1 : 0.5

        onClicked: {
            title.text = "Basemaps";
            mapView.visible = false;
            basemapsGrid.enabled = true;
            gridFadeIn.running = true;
        }
    }

    GridView {
        id: basemapsGrid
        anchors {
            top: title.bottom;
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right
        }
        cellWidth: 128;
        cellHeight: 128
        opacity: 0
        focus: true
        clip: true

        delegate: Rectangle {
            anchors.margins: 5
            width: basemapsGrid.cellWidth
            height: width
            border {
                width: 2;
                color: index === basemapsGrid.currentIndex ? "blue" : "lightgrey"
            }
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
                width: height
                source: thumbnailUrl
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
                horizontalAlignment: Text.AlignHCenter
                text: title
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                font.pointSize: 8
                font.bold: index === basemapsGrid.currentIndex
            }
            //! [BasemapListModel example QML delegate]

            MouseArea {
                enabled: !mapView.visible && portal.loadStatus === Enums.LoadStatusLoaded
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
                    chooseBasemap(basemapsGrid.model.get(index));
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

    Button {
        id: anonymousLogIn
        anchors {
            margins: 16
            horizontalCenter: parent.horizontalCenter
            top: title.bottom
        }
        text: "Anonymous"
        icon.source: "qrc:/Samples/CloudAndPortal/ShowOrgBasemaps/ic_menu_help_dark.png"

        onClicked: {
            portal.load();
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
            portal.credential = oAuthCredential;
            portal.load();
            anonymousLogIn.visible = false;
            userLogIn.visible = false;
        }
    }

    /*
    // Uncomment this section when running as standalone application
    AuthenticationView {
        authenticationManager: AuthenticationManager
    }
    */
}
