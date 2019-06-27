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
import Esri.ArcGISRuntime 100.6
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.6

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url arcgis_url: "http://www.arcgis.com"
    property var portalItem
    property var portalItemListModel
    property var listModelForComboBox : []

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapTopographic {}
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
                background: Rectangle {
                    color: "white"
                }
            }

            Row {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                CheckBox {
                    id: forceLoginBox
                    checked: false
                }
                Text {
                    text: qsTr("Force login")
                    height: forceLoginBox.height
                    verticalAlignment: Text.AlignVCenter
                    color: "#ffffff"
                }
            }

            Row {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.margins: 2
                spacing: 3

                Button {
                    text: qsTr("Search Public")
                    onClicked: {
                        searchPortal(arcgis_url, false);
                    }
                }
                Button {
                    text: qsTr("Search Secure")
                    onClicked: {
                        if (securePortalUrl.text) {
                            searchPortal(securePortalUrl.text, forceLoginBox.checked);
                        } else {
                            webMapMsg.text = "Portal URL is empty. Please enter a portal URL"
                            webMapMsg.visible = true;
                            return;
                        }
                    }
                }
            }

            ComboBox {
                id:webmapsList
                Layout.margins: 2
                Layout.fillWidth: true
                model: null
            }

            Button {
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 2
                onClicked: {
                    if (webmapsList.currentIndex >= 0) {
                        indicator.running = true;
                        loadSelectedWebmap(portalItemListModel.get(webmapsList.currentIndex));
                    }
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

    function searchPortal (portalUrl, forceLogin) {

        var portal;
        if (forceLogin) {
            portal = ArcGISRuntimeEnvironment.createObject("Portal", {url: portalUrl, loginRequired: forceLogin});
        } else {
            portal = ArcGISRuntimeEnvironment.createObject("Portal", {url: portalUrl});
        }

        portal.loadStatusChanged.connect(function() {
            if (portal.loadStatus === Enums.LoadStatusFailedToLoad) {
                webMapMsg.text = portal.loadError.message;
                webMapMsg.visible = true;
                indicator.running = false;
                return;
            }

            if (portal.loadStatus === Enums.LoadStatusLoaded){
                portal.findItems(webmapQuery);
                return;
            }

        });

        portal.findItemsStatusChanged.connect(function() {
            if ( portal.findItemsStatus === Enums.TaskStatusCompleted ) {
                indicator.running = false;
                portalItemListModel = portal.findItemsResult.itemResults;
                var index = 0
                var error = portalItemListModel.forEach(function(prtlItem) {
                    listModelForComboBox[index] = prtlItem.title;
                    index++;
                });

                if (error) {
                    webMapMsg.text = error.message;
                    webMapMsg.visible = true;
                }
                webmapsList.model = listModelForComboBox;
            }
        });

        portal.load();
        indicator.running = true;
    }

    function loadSelectedWebmap(selectedWebmap) {
        portalItem = selectedWebmap;

        portalItem.loadStatusChanged.connect(createMap);
        portalItem.loadErrorChanged.connect( function() {
            webMapMsg.text = portalItem.loadError.message;
            webMapMsg.visible = true;
        });

        portalItem.load();
        if (portalItem.loadStatus === Enums.LoadStatusLoaded){
            createMap();
        }
    }

    function createMap() {
        if (portalItem.loadStatus !== Enums.LoadStatusLoaded)
            return;

        mapView.map = ArcGISRuntimeEnvironment.createObject("Map", {"item": portalItem});

        mapView.map.loadStatusChanged.connect(assignWebmap);
        mapView.map.loadErrorChanged.connect( function() {
            webMapMsg.text = mapView.map.loadError.message;
            webMapMsg.visible = true;
        });

        mapView.map.load();
    }

    function assignWebmap() {
        if (mapView.map.loadStatus !== Enums.LoadStatusLoaded)
            return;

        mapView.visible = true;
        indicator.running = false;
    }

    // Uncomment this section when running as standalone application
    /*
    AuthenticationView {
        authenticationManager: AuthenticationManager
    }*/

    Dialog {
        id: webMapMsg
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: qsTr("Could not load web map!")
        property alias text : textLabel.text
        Text {
            id: textLabel
        }
        onAccepted: visible = false;
    }
}
