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
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

PortalUserInfoSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    onLoadFailed: failAnimation.running = true;

    Column {
        id: loginColumn
        visible: !loaded

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
                load(userNameBox.text, passwordBox.text);
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
            text: loadErrorMessage
            color: "red"
            font.bold: true
        }
    }

    property var detailNames: ["Full name", "Username", "Email", "Bio", "Who can see your profile?"]
    property var detailValue: ["fullName", "username", "email", "bio", "access"]

    Column {
        id: userDetailsColumn
        visible: loaded
        anchors{ top: parent.top; left: parent.left; right: parent.right; margins: 10 * scaleFactor}
        spacing: 10 * scaleFactor

        Text {
            text: fullName.length > 0 ? fullName + " Profile" : ("????")
            font.bold: true
            font.pointSize: 15
        }

        Image {
            source : thumbnailUrl
            height: 32 * scaleFactor
            width: 32 * scaleFactor
        }
    }

    ListView {
        visible: loaded
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
                text: rootRectangle[detailValue[index]]
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
