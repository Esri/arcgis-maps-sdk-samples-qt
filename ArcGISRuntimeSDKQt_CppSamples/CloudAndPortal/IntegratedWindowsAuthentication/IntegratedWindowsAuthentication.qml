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
import Esri.ArcGISRuntime.Toolkit 100.15

Item {
    // Declare the C++ instance which creates the scene etc. and supply the view
    IntegratedWindowsAuthenticationSample {
        id: integratedWindowsAuthenticationSampleModel
        mapView: view
    }

    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
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

            visible: !webmapsList.model

            Text {
                text: qsTr("Enter portal URL secured by IWA")
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
                            integratedWindowsAuthenticationSampleModel.searchIwaSecurePortal(securePortalUrl.text);
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
            visible: webmapsList.model

            Text {
                id: header
                text: "Connected to:"
                Layout.fillWidth: true
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
                textRole: qsTr("title")
                model: integratedWindowsAuthenticationSampleModel.webmapListModel
            }

            Button {
                text: qsTr("Load Web Map")
                Layout.fillWidth: true
                Layout.margins: 2
                visible: webmapsList.model
                onClicked: integratedWindowsAuthenticationSampleModel.loadSelectedWebmap(webmapsList.currentIndex);
            }
        }
    }

    BusyIndicator {
        id: indicator
        anchors.centerIn: parent
        running: integratedWindowsAuthenticationSampleModel.isLoading
    }

    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
        anchors.fill: parent
    }

    Dialog {
        id: webMapMsg
        anchors.centerIn: parent
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        modal: true

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
}
