// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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

import QtQuick 2.15
import QtQuick.Layouts


Rectangle {
    property bool hovered: false
    property bool enabled: true
    property real buttonOpacity: 1.0
    property color hoverColor: "#30111111"
    property color normalColor: "transparent"
    property color textColor: "white"
    property var onClickedHandler
    property alias iconSource: icon.source
    property alias label: labelText.text

    color: hovered ? hoverColor : normalColor
    border.color: "#888"
    border.width: 1
    radius: 6
    height: 40
    width: 100
    opacity: buttonOpacity
    Row {
        anchors.centerIn: parent
        spacing: 3
        Rectangle {
            width: 28; height: 28
            radius: 6
            color: "white"
            border.width: 0
            Image {
                id: icon
                anchors.centerIn: parent
                width: 24; height: 24
            }
        }
        Text {
            id: labelText
            color: parent.parent.textColor
            font.bold: true
            font.pixelSize: 10
            anchors.verticalCenter: parent.verticalCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    MouseArea {
        hoverEnabled: true
        onEntered: parent.hovered = true
        onExited: parent.hovered = false
        anchors.fill: parent
        enabled: parent.enabled
        onClicked: if (parent.onClickedHandler) parent.onClickedHandler()
        cursorShape: Qt.PointingHandCursor
    }
}
