// [WriteFile Name=QueryMapImageSublayer, Category=Layers]
// [Legal]
// Copyright 2018 Esri.
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
import QtQuick.Window
import QtQuick.Controls
import Esri.Samples

QueryMapImageSublayerSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600



    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        anchors {
            fill: controlColumn
            margins: -5
        }
        color: "#efefef"
        radius: 5
        border {
            color: "darkgray"
            width: 1
        }
    }

    Column {
        id: controlColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Row {
            spacing: 5
            Text {
                id: fieldText
                anchors.verticalCenter: parent.verticalCenter
                text: "POP2000 >"
            }

            TextField {
                id: populationText
                anchors.verticalCenter: parent.verticalCenter
                width: 100
                text: "1100000"
                selectByMouse: true
                validator: IntValidator{}
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Query in extent"
            onClicked: query(fieldText.text + populationText.text);
        }
    }
}
