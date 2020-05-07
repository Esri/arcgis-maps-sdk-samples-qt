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
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Item {

    MapView {
        id: view
        anchors.fill: parent
    }

    Rectangle {
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.margins: 2
                spacing: 3

                Button {
                    text: qsTr("Search Public")
                    onClicked: {
                        integratedWindowsAuthenticationSampleModel.searchPublicPortal();
                    }
                }
                Button {
                    text: qsTr("Search Secure")
                    onClicked: {
                        if (securePortalUrl.text) {
                            integratedWindowsAuthenticationSampleModel.searchIwaSecurePortal(securePortalUrl.text);
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
                textRole: qsTr("title")
                model: integratedWindowsAuthenticationSampleModel.webmapListModel
                visible: true
            }

            Button {
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 2
                enabled: webmapsList.model ? true : false
                onClicked: integratedWindowsAuthenticationSampleModel.loadSelectedWebmap(webmapsList.currentIndex);
            }
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        running: integratedWindowsAuthenticationSampleModel.isLoading
    }

    // Uncomment this section when running as standalone application
    /*
    AuthenticationView {
        authenticationManager: integratedWindowsAuthenticationSampleModel.authManager
    }*/

    Dialog {
        id: webMapMsg
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        standardButtons: Dialog.Ok
        title: "Could not load web map!"
        visible: integratedWindowsAuthenticationSampleModel.mapLoadError.length > 0
        ColumnLayout {
            Text {
                id: textLabel
                text: integratedWindowsAuthenticationSampleModel.mapLoadError
            }
            Text {
                id: detailsLabel
            }
        }
        onAccepted: integratedWindowsAuthenticationSampleModel.errorAccepted();
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    IntegratedWindowsAuthenticationSample {
        id: integratedWindowsAuthenticationSampleModel
        mapView: view
    }
}
