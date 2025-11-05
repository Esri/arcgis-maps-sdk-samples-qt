// [WriteFile Name=IntegratedWindowsAuthentication, Category=CloudAndPortal]
// [Legal]
// Copyright 2019 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

Item {
    // Declare the C++ instance which creates the map etc. and supply the view
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
        color: palette.base
        opacity: .9
        radius: 5

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        ColumnLayout {
            id: enterPortalPrompt
            anchors {
                fill: parent
                margins: 5
            }

            visible: !webmapsList.model

            Label {
                text: qsTr("Enter portal URL secured by IWA")
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

            Label {
                id: header
                text: "Connected to:"
                Layout.fillWidth: true
                font {
                    bold: true
                }
            }

            Label {
                id: portalName
                Layout.fillWidth: true
                text: securePortalUrl.text
                horizontalAlignment: Text.AlignLeft
                elide: Text.ElideMiddle
                font {
                    bold: true
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
        visible: integratedWindowsAuthenticationSampleModel.isLoading
    }

    // Declare Authenticator to handle any authentication challenges
    Authenticator {
        anchors.fill: parent
    }

    Dialog {
        id: webMapMsg
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        contentWidth: parent.width * 0.5
        property alias text : textLabel.text
        property alias informativeText : detailsLabel.text
        modal: true
        standardButtons: Dialog.Ok
        title: qsTr("Could not load web map!")
        visible: integratedWindowsAuthenticationSampleModel.mapLoadError.length > 0
        ColumnLayout {
            width: parent.width
            Label {
                id: textLabel
                text: integratedWindowsAuthenticationSampleModel.mapLoadError
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }
            Label {
                id: detailsLabel
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }
        }
        onAccepted: integratedWindowsAuthenticationSampleModel.errorAccepted();
    }
}
