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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.5

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    
    property var user: portal.portalUser

    BusyIndicator {
        id: loadingIndicator
        anchors.centerIn: parent
        running: portal.loadStatus !== Enums.LoadStatusLoaded
    }

    property var detailNames: ["Full name", "Username", "Email", "Bio", "Who can see your profile?"]
    property var detailValue: ["fullName", "username", "email", "userDescription", "access"]

    Column {
        id: userDetailsColumn
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        spacing: 10

        Text {
            text: user ? user.fullName + " Profile" : ("????")
            font.bold: true
            font.pointSize: 15
        }

        Image {
            source : user && user.thumbnailUrl.toString().length > 0 ? user.thumbnailUrl : "qrc:/Samples/CloudAndPortal/PortalUserInfo/placeholder_img.png"
            height: 32
            width: 32
        }
    }

    ListView {
        id: userList
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors {
            top: userDetailsColumn.bottom;
            bottom: midLine.top
            left: parent.left;
            right: parent.right;
            margins: 10
        }
        spacing: 10
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

                    if(detailValue[index] !== "access")
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

    Rectangle {
        id: midLine
        anchors {
            verticalCenter: parent.verticalCenter
            margins: 8
            left: parent.left
            right: parent.right
        }
        height: 4
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        color: "lightgrey"
    }

    Column {
        id: portalDetailsColumn
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors {
            top: midLine.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }
        spacing: 10

        Text {
            text: portal.portalInfo ? portal.portalInfo.organizationName : ""
            font.bold: true
            font.pointSize: 15
        }

        Image {
            source : portal.portalInfo ? portal.portalInfo.thumbnailUrl : ""
            height: 32
            width: 32
        }
    }

    property var infoLabels: ["Description", "Can Find External Content", "Can Share Items Externally"]
    property var infoValues: ["organizationDescription", "canSearchPublic", "canSharePublic"]

    ListView {
        id: infoList
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors {
            top: portalDetailsColumn.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }
        spacing: 10
        clip: true
        model: infoValues.length

        delegate: Column {
            Text {
                text: portal.portalInfo ? infoLabels[index] : ""
                font.bold: true
            }

            Text {
                text: portal.portalInfo ? portal.portalInfo[infoValues[index]] : ""
                color: "grey"
            }
        }
    }

    //! [PortalUserInfo create portal]
    Portal {
        id: portal
        credential: Credential {
            oAuthClientInfo: OAuthClientInfo {
                oAuthMode: Enums.OAuthModeUser
                clientId: "iLkGIj0nX8A4EJda"
            }
        }

        Component.onCompleted: load();

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad)
                retryLoad();
        }
    }

    /* Uncomment this section when running as standalone application
    AuthenticationView {
        id: authView
        authenticationManager: AuthenticationManager
    }
    */
    //! [PortalUserInfo create portal]
}
