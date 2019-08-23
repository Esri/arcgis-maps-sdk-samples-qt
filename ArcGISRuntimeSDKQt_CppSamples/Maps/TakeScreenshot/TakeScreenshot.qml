// [WriteFile Name=TakeScreenshot, Category=Maps]
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
import QtGraphicalEffects 1.0
import Esri.Samples 1.0

TakeScreenshotSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    
    
    // Declare a MapView
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Button {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                margins: 10
            }
            text: "Take screenshot"
            onClicked: {
                captureScreenshot();
                busyIndicator.visible = true;
            }
        }
    }

    onMapImageUrlChanged: {
        imageRect.visible = true;
        busyIndicator.visible = false;
    }

    Rectangle {
        id: imageRect
        anchors.fill: parent
        visible: false
        RadialGradient {
            anchors.fill: parent
            opacity: 0.7
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        Image {
            id: mapImage
            anchors.centerIn: parent
            width: parent.width * 0.75
            height: parent.height * 0.75
            source: mapImageUrl // bind to the property exposed by TakeScreenshot

            Rectangle {
                anchors {
                    right: parent.right
                    top: parent.top
                    margins: 10
                }
                width: 28
                height: width
                color: "lightgray"
                radius: 50

                Image {
                    anchors.centerIn: parent
                    width: parent.width * 0.95
                    height: parent.height * 0.95
                    source: "qrc:/Samples/Maps/TakeScreenshot/close.png"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: imageRect.visible = false;
                }
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
    }
}
