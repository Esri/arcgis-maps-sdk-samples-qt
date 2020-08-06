// [WriteFile Name=EditWithBranchVersioning, Category=EditData]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Layouts 1.12
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Button {
            id: creatVersion
            text: qsTr("Create Version")
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }

            onClicked: {
                if (text === qsTr("Create Version")) {
                    model.createVersion();
                    text = qsTr("Switch to Default version")
                } else if (text === qsTr("Switch to Default version")) {
                    text = qsTr("Switch to created version")
                    model.switchVersion();
                } else if (text === qsTr("Switch to created version")) {
                    text = qsTr("Switch to Default version")
                    model.switchVersion();
                }
            }
        }

        // to be removed
        Button {
            id: fetchVersions
            text: qsTr("Fetch Versions")
            anchors {
                left: parent.left
                top: creatVersion.bottom
                margins: 3
            }

            onClicked: model.fetchVersions();
        }

        Rectangle {
            id: currentVersionRect
            anchors{
                top: parent.top
                horizontalCenter: parent.horizontalCenter
                margins: 3
            }
            width: childrenRect.width;
            height: childrenRect.height;
            color: "#000000"
            visible: currentVersionText.text !== "" ? true : false

            ColumnLayout {
                spacing: 3
                Text {
                    text: qsTr("Current version:")
                    Layout.alignment: Qt.AlignHCenter
                    //                visible: currentVersionText.text !== "" ? true : false
                    color: "white"
                }

                Text {
                    id: currentVersionText
                    Layout.alignment: Qt.AlignHCenter
                    text: model.sgdbCurrentVersion
                    color: "white"
                }
            }
        }
    }

    // Uncomment this section when running as standalone application

    AuthenticationView {
        authenticationManager: model.authManager
    }


    // Declare the C++ instance which creates the scene etc. and supply the view
    EditWithBranchVersioningSample {
        id: model
        mapView: view
    }
}
