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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.ArcGISRuntime
import Esri.ArcGISRuntime.Toolkit

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
        id: connectionBox
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: 275
        height: 175
        color: "#000000"
        opacity: .70
        radius: 5

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        ColumnLayout {
            id: enterPortalPrompt
            anchors {
                fill: parent
                margins: 5
            }

            visible: webmapsList.count === 0

            Text {
                text: qsTr("Enter portal url secured by IWA")
                color: "white"
                font {
                    bold: true
                    pixelSize: 14
                }
            }


            TextField {
                id: securePortalUrl
                Layout.fillWidth: true
                Layout.margins: 2
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
            id: selectMapPrompt
            anchors {
                fill: parent
                margins: 5
            }
            visible: webmapsList.count > 0
            Text {
                id: header
                text: "Connected to:"
                color: "white"
                font {
                    bold: true
                    pointSize: 14
                }
            }

            Text {
                id: portalName
                Layout.fillWidth: true
                text: securePortalUrl.text
                horizontalAlignment: Text.AlignLeft
                elide: Text.ElideMiddle
                color: "white"
                font {
                    bold: true
                    pointSize: 14
                }
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
                onClicked: {
                    const selectedWebmap = webmapsList.model.get(webmapsList.currentIndex);
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

    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
        anchors.fill: parent
    }

    Dialog {
        id: webMapMsg
        anchors.centerIn: parent
        property alias text : textLabel.text
        modal: true

        standardButtons: Dialog.Ok
        title: qsTr("Could not load web map!")
        Text {
            id: textLabel
        }
        onAccepted: visible = false;
    }
}
