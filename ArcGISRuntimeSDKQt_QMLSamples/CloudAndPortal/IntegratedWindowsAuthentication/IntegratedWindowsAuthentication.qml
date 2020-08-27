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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url arcgis_url: "https://www.arcgis.com"
    property var portalItem
    property var iwaSecurePortal

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
        }
    }

    Portal {
        id: publicPortal
        url: arcgis_url
        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad) {
                webMapMsg.text = loadError.message;
                webMapMsg.visible = true;
                indicator.running = false;
                return;
            }

            if (loadStatus === Enums.LoadStatusLoaded){
                findItems(webmapQuery);
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
        id: portalLayoutRect
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .70
        radius: 5

        ColumnLayout {
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
                id: myRow
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.margins: 2
                spacing: 3

                Button {
                    text: qsTr("Search Public")
                    onClicked: {
                        if (publicPortal.loadStatus !== Enums.LoadStatusLoaded) {
                            indicator.running = true;
                            publicPortal.load();
                        } else {
                            publicPortal.findItems(webmapQuery);
                        }
                    }
                }
                Button {
                    text: qsTr("Search Secure")
                    onClicked: {
                        if (securePortalUrl.text) {
                            searchPortal(securePortalUrl.text);
                        } else {
                            webMapMsg.text = "Portal URL is empty. Please enter a portal URL"
                            webMapMsg.visible = true;
                            return;
                        }
                    }
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
                enabled: webmapsList.model ? true : false
                onClicked: loadSelectedWebmap(webmapsList.model.get(webmapsList.currentIndex));
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

    function searchPortal (portalUrl) {

        if (!iwaSecurePortal) {
            iwaSecurePortal = ArcGISRuntimeEnvironment.createObject("Portal", {url: portalUrl});
        } else if (iwaSecurePortal && (iwaSecurePortal.loadStatus === Enums.LoadStatusFailedToLoad)) {
            iwaSecurePortal = ArcGISRuntimeEnvironment.createObject("Portal", {url: portalUrl});
        } else if (iwaSecurePortal && (iwaSecurePortal.loadStatus === Enums.LoadStatusLoaded)) {
            if (iwaSecurePortal.url != portalUrl) {
                iwaSecurePortal = ArcGISRuntimeEnvironment.createObject("Portal", {url: portalUrl});
            } else {
                indicator.running = true;
                iwaSecurePortal.findItems(webmapQuery);
                return;
            }
        }

        iwaSecurePortal.loadStatusChanged.connect(()=> {
            if (iwaSecurePortal.loadStatus === Enums.LoadStatusFailedToLoad) {
                webMapMsg.text = iwaSecurePortal.loadError.message;
                webmapsList.model = null;
                webMapMsg.visible = true;
                indicator.running = false;
                return;
            }

            if (iwaSecurePortal.loadStatus === Enums.LoadStatusLoaded){
                iwaSecurePortal.findItems(webmapQuery);
                return;
            }

        });

        iwaSecurePortal.findItemsStatusChanged.connect(()=> {
            if ( iwaSecurePortal.findItemsStatus === Enums.TaskStatusCompleted ) {
                indicator.running = false;
                webmapsList.textRole = "title";
                webmapsList.model = iwaSecurePortal.findItemsResult.itemResults;
            }
        });

        iwaSecurePortal.load();
        indicator.running = true;
    }

    function loadSelectedWebmap(selectedWebmap) {
        mapView.map = ArcGISRuntimeEnvironment.createObject("Map", {"item": selectedWebmap});
    }

    // Uncomment this section when running as standalone application
    /*
    AuthenticationView {
        authenticationManager: AuthenticationManager
    }*/

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
