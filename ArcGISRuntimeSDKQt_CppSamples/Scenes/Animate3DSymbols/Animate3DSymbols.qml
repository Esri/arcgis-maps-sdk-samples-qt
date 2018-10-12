// [WriteFile Name=Animate3DSymbols, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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
import Esri.ArcGISExtras 1.1

Animate3DSymbolsSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D"
    property bool following: followButton.checked

    missionFrame: progressSlider.value
    zoom: cameraDistance.to - cameraDistance.value
    angle: cameraAngle.value

    onNextFrameRequested: {
        progressSlider.value = progressSlider.value + 1;
        if (progressSlider.value >= missionSize)
            progressSlider.value = 0;
    }

    onWidthChanged: viewWidthChanged((sceneView.width - mapView.width) > mapView.width);

    SceneView {
        id: sceneView
        objectName: "sceneView"
        anchors.fill: parent
    }

    GroupBox {
        id: animationGroupBox
        z: 110
        anchors {
            top: sceneView.top
            left: sceneView.left
            margins: 10 * scaleFactor
        }

        Column {
            spacing: 10

            ComboBox {
                id: missionList
                enabled: !playButton.checked
                model: missionsModel()
                textRole: "display"
                width: 150 * scaleFactor
                onCurrentTextChanged: {
                    changeMission(currentText);
                    progressSlider.value = 0;
                }
            }

            Button {
                id: playButton
                checked: false
                checkable: true
                enabled: missionReady
                text: checked ? "pause" : "play"
            }

            Text {
                id: progressTitle
                text: "progress"
                color: "white"
            }

            Slider {
                id: progressSlider
                from: 0
                to: missionSize
                enabled : missionReady
                width: Math.max(implicitWidth, 150) * scaleFactor
            }

            CheckBox {
                id: followButton
                enabled: missionReady
                text: "follow"
                checked: true

                onCheckedChanged: setFollowing(checked);
            }
        }
    }

    GroupBox {
        id: cameraGroupBox
        z: 110
        anchors {
            top: sceneView.top
            right: sceneView.right
            margins: 10 * scaleFactor
        }

        Column {
            spacing: 10

            Text {
                id: distTitle
                text: "zoom"
                enabled: following && missionReady
                color: "white"
            }

            Slider {
                id: cameraDistance
                enabled: following && missionReady
                from: 10.0
                to: 5000.0
                value: 500.0
                width: Math.max(implicitWidth, 100) * scaleFactor
            }

            Text {
                id: angleTitle
                text: "angle"
                enabled: following && missionReady
                color: "white"
            }

            Slider {
                id: cameraAngle
                enabled: following && missionReady
                from: 0.0
                to: 180.0
                value: 45.0
                width: Math.max(implicitWidth, 100) * scaleFactor
            }

            Text {
                id: speedTitle
                text: "speed"
                enabled: missionReady
                color: "white"
            }

            Slider {
                id: animationSpeed
                enabled: missionReady
                from: 1
                to: 100
                value: 50
                width: Math.max(implicitWidth, 100) * scaleFactor
            }
        }
    }

    Rectangle {
        id: mapFrame
        anchors {
            left:sceneView.left
            bottom: sceneView.bottom
            margins: 10 * scaleFactor
            bottomMargin: 25 * scaleFactor
        }
        width: Math.max(sceneView.width * .2, 128 * scaleFactor)
        height: Math.max(sceneView.height * .4, 128 * scaleFactor)
        color: "black"
        z: 100
        clip: true

        GroupBox {
            id: mapZoomBox
            z: 120
            anchors {
                top: mapFrame.top
                margins: 10 * scaleFactor
            }
            width: mapFrame.width

            Row {
                spacing: 10

                Button {
                    id: mapZoomIn
                    anchors.margins: 10
                    text: "+"
                    width: height
                    onClicked: zoomMapIn()
                }

                Button {
                    id: mapZoomOut
                    anchors.margins: 10
                    text: "-"
                    width: height
                    onClicked: zoomMapOut()
                }
            }
        }

        MapView {
            id: mapView
            objectName: "mapView"
            anchors {
                fill: mapFrame
                margins: 2 * scaleFactor
            }

            MouseArea {
                anchors.fill: parent
                onPressed: mouse.accepted
                onWheel: wheel.accepted
            }
        }
    }

    Timer {
        id: timer
        interval: Math.max(animationSpeed.to - animationSpeed.value,1);
        running: playButton.checked;
        repeat: true
        onTriggered: animate();
    }
}
