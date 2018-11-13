// [WriteFile Name=Buffer, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Window 2.2
import Esri.Samples 1.0

BufferSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // display a control pane to change size
    Rectangle {
        anchors {
            margins: -5
            fill: controlColumn
        }
        color: "lightgray"
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

        Text {
            text: "Buffer size (miles):"
        }

        TextField {
            id: bufferSizeText
            validator: IntValidator { bottom: 1; top: 10000 }
            text: "1000"
            onTextChanged: bufferSize = Number(text);
        }

        Row {
            spacing: 10

            Rectangle {
                radius: 100
                width: 15
                height: width
                color: "blue"
                opacity: 0.5
            }

            Text {
                text: "Planar Buffer"
            }
        }

        Row {
            spacing: 10

            Rectangle {
                radius: 100
                width: 15
                height: width
                color: "red"
                opacity: 0.5
            }

            Text {
                text: "Geodesic Buffer"
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Clear"
            onClicked: {
                clear();
            }
        }
    }
}
