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
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Dialogs 2.0

Rectangle {
    id: rootRectangle
    clip: true

    width: 500
    height: 500

    property real scaleFactor: System.displayScaleFactor
    property var user

    Portal {
        id: portal
        credential: usersCredential
        loginRequired: true

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusFailedToLoad) {
                console.log(portal.loadError.message);
                failAnimation.running = true;
                return;
            }
            else if(loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }
            user = portalUser
        }
    }

    Credential {
        id: usersCredential
        username: userNameBox.text
        password: passwordBox.text
    }

    Column {
        id: loginColumn
        visible: portal.loadStatus !== Enums.LoadStatusLoaded

        anchors {fill: parent; margins: 10 * scaleFactor }
        spacing: 10 * scaleFactor


        Column {
            Text {
                text: qsTr("Username")
                font.bold: true
            }

            TextField {
                id: userNameBox
                placeholderText: "enter Username"
                style: TextFieldStyle {
                    textColor: "black"
                    background: Rectangle {
                        radius: 4
                        color: "lightyellow"
                        border.color: "lightgrey"
                        border.width: 1
                    }
                }
            }
        }

        Column {
            Text {
                text: qsTr("Password")
                font.bold: true
            }

            TextField {
                id: passwordBox
                placeholderText: "enter Password"
                style: TextFieldStyle {
                    textColor: "black"
                    background: Rectangle {
                        radius: 4
                        color: "lightyellow"
                        border.color: "lightgrey"
                        border.width: 1
                    }
                }
                echoMode: TextInput.Password
            }
        }

        Button {
            id: loadButton
            enabled: passwordBox.text.length > 0 && userNameBox.text.length > 0
            style: ButtonStyle {
                background: Rectangle {
                    border.color: "lightgrey"
                    color: "orange"
                    radius: 4
                }
                label: Text {
                    text: qsTr("SIGN IN")
                    color: "white"
                }
            }

            onClicked: {
                if (portal.loadStatus === Enums.LoadStatusNotLoaded)
                    portal.load();
                else if (portal.loadStatus === Enums.LoadStatusFailedToLoad)
                    portal.retryLoad();
            }

            SequentialAnimation on x {
                id: failAnimation
                loops: 10
                running: false
                PropertyAnimation { to: 50; duration: 20 }
                PropertyAnimation { to: 0; duration: 20 }
            }
        }

        Text {
            text: portal.loadError ? portal.loadError.message : ""
            color: "red"
            font.bold: true
        }
    }

    Column {
        id: userDetailsColumn
        visible: portal.loadStatus === Enums.LoadStatusLoaded
        anchors {fill: parent; margins: 10 * scaleFactor }
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

        Column {
            Text {
                text: "Full Name"
                font.bold: true
            }

            Text {
                text: user ? user.fullName: ("????")
                color: "grey"
            }
        }

        Column {
            Text {
                text: "Username"
                font.bold: true
            }

            Text {
                text: user ? user.username: ("????")
                color: "grey"
            }
        }

        Column {
            Text {
                text: "Email"
                font.bold: true
            }

            Text {
                text: user ? user.email: ("????")
                color: "grey"
            }
        }

        Column {
            Text {
                text: "Bio"
                font.bold: true
            }

            Text {
                text: user ? user.description: ("????")
                color: "grey"
            }
        }

        Column {
            Text {
                text: "Who can see your profile"
                font.bold: true
            }

            Text {
                text: {
                    if (!user)
                        return "????";
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
