// Copyright 2019 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.5

// Create the download button to export the tile cache
Rectangle {
    property bool pressed: false
    signal buttonClicked()

    width: 190
    height: 35
    color: pressed ? "#959595" : "#D6D6D6"
    radius: 5
    border {
        color: "#585858"
        width: 1
    }

    Row {
        anchors.fill: parent
        spacing: 5
        Image {
            width: 38
            height: width
            source: "qrc:/Samples/Maps/GenerateOfflineMapLocalBasemap/download.png"
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "Generate Offline Map"
            font.pixelSize: 14
            color: "#474747"
        }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: downloadButton.pressed = true
        onReleased: downloadButton.pressed = false
        onClicked: {
            buttonClicked();
        }
    }
}
