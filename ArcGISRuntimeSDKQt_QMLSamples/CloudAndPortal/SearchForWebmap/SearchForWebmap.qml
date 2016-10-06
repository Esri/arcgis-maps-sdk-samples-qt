// [WriteFile Name=SearchForWebmap, Category=CloudAndPortal]
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
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    function search(keyWord) {
        console.log("here");
        webmapsModel.append({"name": "Imagery"});
        webmapsModel.append({"name": "Imagery with labels"});
        webmapsModel.append({"name": "Light Gray Canvas"});
        webmapsModel.append({"name": "National Geographic"});
        webmapsModel.append({"name": "Oceans"});
        webmapsModel.append({"name": "Streets"});
        webmapsModel.append({"name": "Terrain with labels"});
        webmapsModel.append({"name": "Topographic"});

        webmapsList.model = webmapsModel;
        webmapsList.visible = true;

        console.log(webmapsList.model.count);
    }

    ListModel {
        id: webmapsModel
    }

    Portal {
        id: portal
        loginRequired: true
        credential: Credential {
            username: "h"
            password: "h"
        }

        Component.onCompleted: load();

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad)
                retryLoad();

            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            searchBox.visible = true
        }
    }

    Component {
        id: webmapDelegate
        Rectangle {
            width: webmapsList.width
            height: 50
            color: "lightgrey"
            border {
                width: 2
                color: "black"
            }
            radius: 4

            Text {
                text: name
                font.bold: index === webmapsList.currentIndex
            }
        }
    }

    ListView {
        id: webmapsList
        anchors {top: searchBox.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
        visible: true
//        clip: true

        delegate: webmapDelegate
    }

    Column {
        id: searchBox
        anchors {top: parent.top; horizontalCenter: parent.horizontalCenter; margins: 10 * scaleFactor}
        visible: true
        height: 100
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
        }
    }

//    MapView {
//        id: mapView
//        visible: false
//        anchors {top: searchBox.bottom; bottom: parent.bottom; left: parent.left; margins: 10 * scaleFactor}
//    }



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
