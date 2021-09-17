// [WriteFile Name=IntegratedWindowsAuthentication, Category=CloudAndPortal]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.13
import Esri.ArcGISRuntime.Toolkit 100.13

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: webMap
            Basemap {
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
        }
    }

    Portal {
        id: iwaSecurePortal
        loginRequired: true

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusLoading) {
                indicator.running = true;
                return;
            }

            else if (loadStatus === Enums.LoadStatusLoaded) {
                indicator.running = false;
                findItems(webmapQuery);
                return;
            }

            else if (loadStatus === Enums.LoadStatusFailedToLoad) {
                webMapMsg.text = loadError.message;
                webmapsList.model = null;
                webMapMsg.visible = true;
                indicator.running = false;
                return;
            }
        }

        onFindItemsStatusChanged: {
            if ( findItemsStatus === Enums.TaskStatusCompleted ) {
                indicator.running = false;
                webmapsList.textRole = "title";
                webmapsList.model = findItemsResult.itemResults;
            }
        }
    }

    Rectangle {
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: 275
        height: 150
        color: "#000000"
        opacity: .70
        radius: 5

        ColumnLayout {
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
            width: 250
            visible: webmapsList.model ? false : true

            TextField {
                id: securePortalUrl
                Layout.fillWidth: true
                Layout.margins: 2
                placeholderText: qsTr("Enter portal url secured by IWA")
                selectByMouse: true

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "white"
                }
            }

            Row {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.margins: 2
                spacing: 3

                Button {
                    text: qsTr("Search Secure")
                    onClicked: {
                        if (securePortalUrl.text) {
                            iwaSecurePortal.url = securePortalUrl.text;
                            iwaSecurePortal.load();
                        } else {
                            webMapMsg.text = "Portal URL is empty. Please enter a portal URL"
                            webMapMsg.visible = true;
                            return;
                        }
                    }
                }
            }
        }

        ColumnLayout {
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
            width: 250
            visible: webmapsList.model ? true : false

            Text {
                id: header
                text: "Connected to:"
                color: "white"
                font.bold: true
                font.pointSize: 14
            }

            Text {
                id: portalName
                text: securePortalUrl.text
                elide: Text.ElideMiddle
                color: "white"
                font.bold: true
                font.pointSize: 14
            }

            ComboBox {
                id: webmapsList
                Layout.margins: 2
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 2
                visible: webmapsList.model ? true : false
                onClicked: {
                    var selectedWebmap = webmapsList.model.get(webmapsList.currentIndex);
                    mapView.map = ArcGISRuntimeEnvironment.createObject("Map", {"item": selectedWebmap});
                }
            }
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        running: false
    }

    PortalQueryParametersForItems {
        id: webmapQuery
        types: [ Enums.PortalItemTypeWebMap ]
    }

    // Uncomment this section when running as standalone application

    //    AuthenticationView {
    //        anchors.fill: parent
    //    }

    Dialog {
        id: webMapMsg
        property alias text : textLabel.text
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: qsTr("Could not load web map!")
        Text {
            id: textLabel
        }
        onAccepted: visible = false;
    }
}
