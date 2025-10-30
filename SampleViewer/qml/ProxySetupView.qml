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
import Esri.ArcGISRuntimeSamples
import Calcite

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
            color: Calcite.foreground1
        }

        header: ToolBar {
            height: 42

            background: Rectangle {
                color: Calcite.brand
            }

            Label {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Proxy Settings")
                font {
                    family: fontFamily
                    pixelSize: 18
                }
                color: Calcite.offWhite
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
                placeholderTextColor: Calcite.text3
                renderType: Text.NativeRendering
                color: Calcite.text1
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
                placeholderTextColor: Calcite.text3
                renderType: Text.NativeRendering
                color: Calcite.text1
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
                placeholderTextColor: Calcite.text3
                renderType: Text.NativeRendering
                color: Calcite.text1
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
                placeholderTextColor: Calcite.text3
                renderType: Text.NativeRendering
                color: Calcite.text1
                selectByMouse: true
            }

            Rectangle {
                width: parent.width
                height: okButton.height
                color: Calcite.foreground1

                Button {
                    id: okButton
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("Setup")
                    width: 100
                    height: 32

                    onClicked: {
                        if (!msg.visible) {
                            SampleManager.setupProxy(hostName.text, portNumber.text, userName.text, password.text);
                            dialogComponent.visible = false;
                        } else
                            msg.color = Calcite.danger;
                    }
                }

                Button {
                    id : close
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    text: qsTr("Cancel")
                    width: 100
                    height: 32
                    onClicked: dialogComponent.visible = false
                }
            }

            Text {
                id: msg
                text: qsTr("*Required fields")
                color: Calcite.text3
                font.pixelSize: 12
                visible: hostName.text.trim() === "" || portNumber.text.trim() === ""
            }
        }
    }
}
