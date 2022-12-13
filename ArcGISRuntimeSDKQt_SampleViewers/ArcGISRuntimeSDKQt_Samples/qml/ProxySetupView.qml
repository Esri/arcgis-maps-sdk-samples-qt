// [Legal]
// Copyright 2022 Esri.

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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import Esri.ArcGISRuntimeSamples

Item {
    id: dialogComponent
    visible: false

    property string host: hostName.text
    property string port: portNumber.text
    property string user: userName.text
    property string pw: password.text



    Rectangle {
        id: overlay
        anchors.fill: parent
        color: "#60000000"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log(hostName.height, okButton.height)
                dialogComponent.visible = false
            }
        }
    }

    Page {
        id: dialogWindow
        anchors.centerIn: parent
        width: 275
        height: 300
        clip: true
        background: Rectangle {
            radius: 3
        }

        header: ToolBar {
            height: 42

            Label {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Proxy Settings")
                font {
                    family: fontFamily
                    pixelSize: 18
                }
                color: "white"
            }
        }

        Column {
            id: items
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 5

            TextField {
                id: hostName
                width: parent.width
                height: 38
                font {
                    family: fontFamily
                    pixelSize: 14
                }
                placeholderText: qsTr("* Enter host name e.g. 127.0.0.1")
                placeholderTextColor: "darkgray"
                renderType: Text.NativeRendering
                color: Material.primary
                selectByMouse: true
            }

            TextField {
                id: portNumber
                width: parent.width
                height: 38
                font {
                    family: fontFamily
                    pixelSize: 14
                }
                placeholderText: qsTr("* Enter port number e.g. 8888")
                placeholderTextColor: "darkgray"
                renderType: Text.NativeRendering
                color: Material.primary
                selectByMouse: true
            }

            TextField {
                id: userName
                width: parent.width
                height: 38
                font {
                    family: fontFamily
                    pixelSize: 14
                }
                placeholderText: qsTr("Enter user name e.g. my_name")
                placeholderTextColor: "darkgray"
                renderType: Text.NativeRendering
                color: Material.primary
                selectByMouse: true
            }

            TextField {
                id: password
                width: parent.width
                height: 38
                font {
                    family: fontFamily
                    pixelSize: 14
                }
                echoMode: TextInput.Password
                placeholderText: qsTr("Enter password")
                placeholderTextColor: "darkgray"
                renderType: Text.NativeRendering
                color: Material.primary
                selectByMouse: true
            }

            Rectangle {
                width: parent.width
                height: okButton.height

                Button {
                    id: okButton
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("Setup")
                    width: 110
                    height: 48

                    onClicked: {
                        if (!msg.visible) {
                            SampleManager.setupProxy(hostName.text, portNumber.text, userName.text, password.text);
                            dialogComponent.visible = false;
                        } else
                            msg.color = "red";
                    }
                }

                Button {
                    id : close
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("Cancel")
                    width: 110
                    height: 48
                    onClicked: dialogComponent.visible = false
                }
            }

            Text {
                id: msg
                text: qsTr("*Required fields")
                color: "darkgray"
                font.pixelSize: 12
                visible: hostName.text.trim() === "" || portNumber.text.trim() === ""
            }
        }
    }
}
