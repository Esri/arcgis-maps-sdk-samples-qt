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
import QtQuick.Controls 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property var porInfo: portal.portalInfo


    function chooseBasemap(selectedBasemap){
        title.text = selectedBasemap.item.title;
        basemapsGrid.enabled = false;

        var newMap = ArcGISRuntimeEnvironment.createObject("Map", {basemap: selectedBasemap});
        mapView.map = newMap;
        gridFadeOut.running = true;
        mapView.visible = true;
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: !mapView.visible && portal.loadStatus !== Enums.LoadStatusLoaded;
    }

    Portal {
        id: portal

        credential: Credential {
            oAuthClientInfo: OAuthClientInfo {
                oAuthMode: Enums.OAuthModeUser
                clientId: "W3hPKzPbeJ0tr8aj"
            }
        }

        Component.onCompleted: load();

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad)
            {
                retryLoad();
                return;
            }

            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            fetchBasemaps();
        }

        onFetchBasemapsStatusChanged: {
            console.log("fetch basemaps finished");
            if (fetchBasemapsStatus !== Enums.TaskStatusCompleted)
                return;

            basemapsGrid.model = basemaps;
            gridFadeIn.running = true;
        }

        onErrorChanged: console.log(error.message);
    }

    Text{
        id: title
        anchors {top: parent.top; left: parent.left; right: parent.right; margins: 10}
        font.pointSize: 14
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignTop
        text: porInfo ? porInfo.organizationName + " Basemaps" : "Loading Organization Basemaps..."
        wrapMode: Text.Wrap
        elide: Text.ElideRight
    }

    MapView {
        visible: false
        id: mapView

        anchors{top: title.bottom; bottom: parent.bottom; left: parent.left; right: parent.right}
    }

    GridView {
        id: basemapsGrid
        anchors{top: title.bottom; bottom: parent.bottom; left: parent.left; right: parent.right}
        cellWidth: 128 * scaleFactor;
        cellHeight: 128 * scaleFactor
        opacity: 0
        focus: true
        clip: true

        delegate: Rectangle {
            anchors.margins: 5 * scaleFactor
            width: basemapsGrid.cellWidth
            height: width
            border {width: 2; color: index == basemapsGrid.currentIndex ? "blue" : "lightgrey"}
            color: index == basemapsGrid.currentIndex ? "yellow" : "white"
            radius: 2
            clip: true

            Text {
                anchors {fill: parent; margins: 10 * scaleFactor }
                z: 100
                text: item.title
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                font.pointSize: 8
                font.bold: index == basemapsGrid.currentIndex
            }

//            Image {
//                id: basemapImg
//                anchors {fill: parent; margins: 10 * scaleFactor }
//                source: item.thumbnailUrl
//            }

            MouseArea {
                enabled: !mapView.visible && portal.loadStatus == Enums.LoadStatusLoaded
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

    AuthenticationView {
        authenticationManager: AuthenticationManager
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
