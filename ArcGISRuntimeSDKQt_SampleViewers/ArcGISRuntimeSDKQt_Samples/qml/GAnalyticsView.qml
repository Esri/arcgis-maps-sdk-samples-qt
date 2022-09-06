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
import Telemetry
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: dialogComponent
    visible: GAnalytics.isVisible

    Component.onCompleted: {
        dialogComponent.visible = GAnalytics.isVisible;
    }

    RadialGradient {
        id: overlay
        anchors.fill: parent
        opacity: 0.7
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgrey" }
            GradientStop { position: 0.7; color: "black" }
        }

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
        }

        header: ToolBar {
            height: 42

            Label {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Telemetry Settings")
                font {
                    family: fontFamily
                    pixelSize: 18
                }
                color: "white"
            }
        }

        ColumnLayout {
            id: items
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 5

            Text {
                id: msg
                Layout.fillWidth: true
                text: qsTr("This app uses telemetry to determine which samples are being used and what searches are done in app. No location or GPS data is reported using telemetry.")
                wrapMode: Text.WordWrap
                font.pixelSize: 12
            }

            CheckBox {
                id: analyticsEnabledCheckBox
                text: "Enable analytics"
                onCheckedChanged: {
                    GAnalytics.telemetryEnabled = analyticsEnabledCheckBox.checked;
                }
                Component.onCompleted: {
                    checked = GAnalytics.telemetryEnabled;
                }
            }

            Button {
                id : closeButton
                Layout.alignment: Qt.AlignCenter
                text: qsTr("Close")
                width: 110
                height: 48
                onClicked: dialogComponent.visible = false
            }
        }
    }
}

