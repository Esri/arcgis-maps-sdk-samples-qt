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
                placeholderText: qsTr("Enter portal url secured by IWA")
            }

            Row {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                CheckBox {
                    id: forceLoginBox
                    checked: false
                }
                Text {
                    id: forceLoginText
                    text: qsTr("Force login")
                    height: forceLoginBox.height
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 10
                    color: "#ffffff"
                }
            }

            Row {
                Layout.fillWidth: true
                Layout.margins: 3
                spacing: 4
                Button {
                    id: searchPublic
                    text: qsTr("Search Public")
                    onClicked: {
                        integratedWindowsAuthenticationSampleModel.searchPortal(arcgis_url, false);
                    }
                }
                Button {
                    id: searchSecure
                    text: qsTr("Search Secure")
                    onClicked: {
                        integratedWindowsAuthenticationSampleModel.searchPortal(securePortalUrl.text, forceLoginBox.checked);
                    }
                }
            }

            ComboBox {
                id:webmapsList
                Layout.margins: 3
                Layout.fillWidth: true
                model: integratedWindowsAuthenticationSampleModel.webmapListModel
                visible: true
            }

            Button {
                id: loadSelectedWebmapBtn
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 3
                visible: true
                onClicked: {
                    if (webmapsList.currentIndex >= 0) {
                        integratedWindowsAuthenticationSampleModel.loadSelectedWebmap(webmapsList.currentIndex);
                    }

                }
            }
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        running: integratedWindowsAuthenticationSampleModel.isLoading
    }

    // Uncomment this section when running as standalone application
    AuthenticationView {
        authenticationManager: integratedWindowsAuthenticationSampleModel.authManager
    }

    //! [PortalUserInfo create portal]

    Dialog {
        id: webMapMsg
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Could not load web map!"
        visible: integratedWindowsAuthenticationSampleModel.mapLoadError.length > 0
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        ColumnLayout {
            Text {
                id: textLabel
                text: integratedWindowsAuthenticationSampleModel.mapLoadError
            }
            Text {
                id: detailsLabel
            }
        }
        onAccepted: errorAccepted()
    }


    // Declare the C++ instance which creates the scene etc. and supply the view
    IntegratedWindowsAuthenticationSample {
        id: integratedWindowsAuthenticationSampleModel
        mapView: view
    }
}
