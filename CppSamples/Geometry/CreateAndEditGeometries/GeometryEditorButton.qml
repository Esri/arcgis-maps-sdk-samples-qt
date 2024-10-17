// [WriteFile Name=CreateAndEditGeometries, Category=Geometry]
// [Legal]
// Copyright 2023 Esri.
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// This component defines each of the buttons in the Geometry Editor control UI

RoundButton {
    id: geometryEditorButton

    property string buttonName: ""
    property string iconPath: ""

    Layout.fillWidth: true

    // Set the focus policy so that the buttons do not take focus from the MapView
    focusPolicy: Qt.NoFocus

    radius: 5

    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        opacity: parent.enabled || parent.checked ? 1 : 0.3
        color: geometryEditorButton.down ? "#d0d0d0" : "#e0e0e0"
    }

    Image {
        id: imgComponent
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -textComponent.height/2
        }
        source: iconPath
        width: 20
        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: textComponent
        anchors {
            top: imgComponent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: buttonName
        font.pixelSize: 8
    }
}
