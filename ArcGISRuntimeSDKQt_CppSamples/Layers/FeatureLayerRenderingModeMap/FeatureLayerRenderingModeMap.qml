// [WriteFile Name=FeatureLayerRenderingModeMap, Category=Layers]
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
import Esri.Samples 1.0

FeatureLayerRenderingModeMapSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    MapView {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2
        objectName: "topMapView"

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                margins: 5
            }
            color: "white"
            radius: 5
            width: 200
            height: 30

            Text {
                anchors.centerIn: parent
                text: "Rendering Mode Static"
            }
        }
    }

    MapView {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 2
        objectName: "bottomMapView"

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                margins: 5
            }
            color: "white"
            radius: 5
            width: 200
            height: 30

            Text {
                anchors.centerIn: parent
                text: "Rendering Mode Dynamic"
            }
        }
    }

    Button {
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        readonly property string startText: "Start Animation"
        readonly property string stopText: "Stop Animation"
        text: startText
        onClicked: {
            if (text === startText) {
                startAnimation();
                text = stopText;
            } else {
                stopAnimation();
                text = startText;
            }
        }
    }
}
