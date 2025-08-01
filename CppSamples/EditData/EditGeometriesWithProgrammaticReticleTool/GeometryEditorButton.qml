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
import QtQuick.Controls 2.15
import QtQuick.Layouts

AbstractButton {
    id: root
    property alias iconSource: icon.source
    property string tooltipText: ""
    property real buttonOpacity: 1.0
    property var onClickedHandler

    width: 140
    height: 56
    opacity: buttonOpacity
    background: Rectangle {
        border.color: "#888"
        border.width: 1
    }
    contentItem: Row {
        anchors.centerIn: parent
        spacing: 6
        Rectangle {
            width: 36; height: 36
            color: "white"
            border.width: 0
            Image {
                id: icon
                anchors.centerIn: parent
                width: 32
                height: 32
            }
        }
    }
    ToolTip.visible: hovered && tooltipText.length > 0
    ToolTip.text: tooltipText
    onClicked: if (onClickedHandler) onClickedHandler()
}
