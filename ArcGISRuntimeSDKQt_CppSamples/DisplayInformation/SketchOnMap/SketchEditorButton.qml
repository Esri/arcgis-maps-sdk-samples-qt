// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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
import QtQuick.Layouts 1.15

// This component defines each of the buttons in the sketch editor control UI

RoundButton {
    id: sketchEditorButton

    property var columnSpan: 1
    property string buttonName: ""
    property string iconPath: ""
    property var images: 1

    Layout.fillWidth: true
    Layout.columnSpan: columnSpan
    radius: 5

    // Set the focus policy so that the buttons do not take focus from the MapView
    focusPolicy: Qt.NoFocus

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
        visible: images === 1
    }

    Image {
        id: imgComponentLeft
        anchors {
            right: parent.horizontalCenter
            rightMargin: -imgComponentRight.width/5
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -textComponent.height/2 - 2
        }
        source: iconPath
        width: 20
        fillMode: Image.PreserveAspectFit

        visible: images === 2
    }

    Image {
        id: imgComponentRight
        anchors {
            left: parent.horizontalCenter
            leftMargin: -imgComponentLeft.width/5
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -textComponent.height/2
        }
        source: iconPath
        width: 20
        fillMode: Image.PreserveAspectFit

        visible: images === 2
    }

    Text {
        id: textComponent
        anchors {
            top: images === 1 ? imgComponent.bottom : imgComponentRight.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: buttonName
        font.pixelSize: 8
    }
}
