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
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.6

Item {

    readonly property url arcgis_url: "http://www.arcgis.com"

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
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
        opacity: .75
        radius: 5

        ColumnLayout {
            TextField {
                id: securePortalUrl
                Layout.fillWidth: true
                Layout.margins: 3
//                text: qsTr("Enter portal url")
                text: qsTr("https://portaliwads.ags.esri.com/gis/")
            }

            Row {
                Layout.fillWidth: true
                Layout.margins: 3
                spacing: 4
                Button {
                    id: searchPublic
                    text: qsTr("Search Public")
                    onClicked: {
                        integratedWindowsAuthenticationSampleModel.searchPortal(arcgis_url);
//                        searchPortal(arcgis_url);
//                        webmapsList.visible = true;
//                        loadSelectedWebmapBtn.visible = true;
                    }
                }
                Button {
                    id: searchSecure
                    text: qsTr("Search Secure")
                    onClicked: {
                        integratedWindowsAuthenticationSampleModel.searchPortal(securePortalUrl.text);
//                        console.log(securePortalUrl.text);
//                        searchPortal(securePortalUrl.text);
//                        webmapsList.visible = true;
//                        loadSelectedWebmapBtn.visible = true;
                    }
                }
            }

            ComboBox {
                id:webmapsList
                Layout.margins: 3
                Layout.fillWidth: true
                model: integratedWindowsAuthenticationSampleModel.webmapListModel
//                visible: true;
            }

            Button {
                id: loadSelectedWebmapBtn
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 3
                visible: false
                onClicked: {
//                    loadSelectedWebmap(portalItemListModel.get(webmapsList.currentIndex));
                }
            }
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        running: false
//        running: publicPortal.loadStatus == Enums.LoadStatusLoading
    }

    // Uncomment this section when running as standalone application
    AuthenticationView {
        authenticationManager: integratedWindowsAuthenticationSampleModel.authManager
    }

    //! [PortalUserInfo create portal]


    // Declare the C++ instance which creates the scene etc. and supply the view
    IntegratedWindowsAuthenticationSample {
        id: integratedWindowsAuthenticationSampleModel
        mapView: view
    }
}
