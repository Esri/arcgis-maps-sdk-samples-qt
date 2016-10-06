// [WriteFile Name=PortalUserInfo, Category=CloudAndPortal]
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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

Rectangle {
    id: rootRectangle
    clip: true

    width: 600
    height: 800

    property real scaleFactor: System.displayScaleFactor
    property var user

    BusyIndicator {
        id: loadingIndicator
        anchors.centerIn: parent
        running: portal.loadStatus !== Enums.LoadStatusLoaded
    }

    //! [PortalUserInfo create portal]
    Portal {
        id: portal
        credential: Credential {
            oAuthClientInfo: OAuthClientInfo {
                oAuthMode: Enums.OAuthModeUser
                clientId: "W3hPKzPbeJ0tr8aj"
            }
        }

        Component.onCompleted: load();

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad) {
                retryLoad();
                return;
            }
            else if(loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }
            user = portalUser
        }
    }

    AuthenticationView {
        id: authView
        authenticationManager: AuthenticationManager
    }
    //! [PortalUserInfo create portal]

    property var detailNames: ["Full name", "Username", "Email", "Bio", "Who can see your profile?"]
    property var detailValue: ["fullName", "username", "email", "description", "access"]

    Column {
        id: userDetailsColumn
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors{ top: parent.top; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
        spacing: 10 * scaleFactor

        Text {
            text: user ? user.fullName + " Profile" : ("????")
            font.bold: true
            font.pointSize: 15
        }

        Image {
            source : user && user.thumbnailUrl.lenghth > 0 ? user.thumbnailUrl : "qrc:/Samples/CloudAndPortal/PortalUserInfo/placeholder_img.png"
            height: 32 * scaleFactor
            width: 32 * scaleFactor
        }
    }

    ListView {
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors{ top: userDetailsColumn.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
        spacing: 10 * scaleFactor
        clip: true
        model: detailNames.length

        delegate: Column {
            Text {
                text: detailNames[index]
                font.bold: true
            }

            Text {
                text: {
                    if (!user)
                        return "????";

                    if( detailValue[index] !== "access")
                        return user[detailValue[index]];

                    if (user.access === Enums.PortalAccessOrganization)
                        return "Organization";
                    else if (user.access === Enums.PortalAccessPrivate)
                        return "Only you";
                    else if (user.access === Enums.PortalAccessPublic)
                        return "Everyone";
                    else if (user.access === Enums.PortalAccessShared)
                        return "Shared Groups";
                    return "????";
                }
                color: "grey"
            }
        }
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
