// [WriteFile Name=FindPlace, Category=Search]
// [Legal]
// Copyright 2017 Esri.

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

TextField {
    width: parent.width
    selectByMouse: true

    property url imageUrl
    signal imageClicked

    Rectangle {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            margins: 5
        }
        height: 22
        width: height

        Image {
            anchors {
                fill: parent
                margins: 2
            }
            source: imageUrl

            MouseArea {
                anchors.fill: parent
                onClicked: imageClicked()
            }
        }
    }
}

