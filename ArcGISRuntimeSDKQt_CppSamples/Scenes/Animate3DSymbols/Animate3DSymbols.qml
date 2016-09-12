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
import QtQuick.Controls 1.4
import QtQuick.Extras 1.4
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

Animate3DSymbolsSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D"

    SceneView {
        id: sceneView
        objectName: "sceneView"
        anchors.fill: parent
        z: 10

        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = followButton.checked
            onWheel: wheel.accepted = followButton.checked
        }
    }

    Button{
        id: playButton
        anchors.top: sceneView.top
        anchors.left: sceneView.left
        width: 64 * scaleFactor
        height: 16 * scaleFactor
        checked: false
        checkable: true
        enabled: missionReady
        z: 110
        text: checked ? "Pause" : "Play"
    }

    ComboBox{
        id: missionList
        model: missionsModel()
        textRole: "display"
        anchors.top: playButton.bottom
        anchors.left: sceneView.left
        width: 64 * scaleFactor
        height: 16 * scaleFactor
        z: 110

        onCurrentTextChanged: changeMission(currentText)
    }

    CheckBox{
        id: followButton
        anchors.top: missionList.bottom
        anchors.left: sceneView.left
        checked: true
        enabled: missionReady
        z: 110
        text: "follow"
        onCheckedChanged: setFollowing(checked);
    }

    Text{
        id: distTitle
        text: "Zoom"
        enabled: followButton.checked && missionReady
        anchors.top: cameraDistance.top
        anchors.right: cameraDistance.left
        anchors.rightMargin: 10
        z: 110
        height: 16 * scaleFactor
        renderType: Text.NativeRendering
    }
    Slider{
        id: cameraDistance
        enabled: followButton.checked && missionReady
        anchors.top: sceneView.top
        anchors.right: sceneView.right
        z: 110
        minimumValue: 10.
        maximumValue: 500.
        value: 200.

        Component.onCompleted: setZoom(value);
        onValueChanged: setZoom(value);
    }

    Text{
        id: angleTitle
        text: "Angle"
        enabled: followButton.checked && missionReady
        anchors.top: cameraAngle.top
        anchors.right: cameraAngle.left
        anchors.rightMargin: 10
        z: 110
        height: 16 * scaleFactor
        renderType: Text.NativeRendering
    }
    Slider{
        id: cameraAngle
        anchors.top: cameraDistance.bottom
        anchors.right: sceneView.right
        z: 110
        enabled: followButton.checked && missionReady
        minimumValue: 0.
        maximumValue: 180.
        value: 75.

        Component.onCompleted: setAngle(value);
        onValueChanged: setAngle(value)
    }

    Text{
        id: speedTitle
        text: "Speed"
        enabled: missionReady
        anchors.top: animationSpeed.top
        anchors.right: animationSpeed.left
        anchors.rightMargin: 10
        z: 110
        height: 16 * scaleFactor
        renderType: Text.NativeRendering
    }
    Slider{
        id: animationSpeed
        anchors.top: cameraAngle.bottom
        anchors.right: sceneView.right
        z: 110
        enabled: missionReady
        minimumValue: 50
        maximumValue: 200
        value: 50
    }

    Rectangle
    {
        anchors.left: sceneView.left
        anchors.bottom: sceneView.bottom
        width: sceneView.width * .2
        height: sceneView.height * .4
        color: "black"
        z: 100

        MapView{
            objectName: "mapView"
            anchors.fill: parent
            anchors.margins: 2

            MouseArea {
                anchors.fill: parent
                onPressed: mouse.accepted = followButton.checked
                onWheel: wheel.accepted = followButton.checked
            }
        }
    }

    Timer{
        id: timer
        interval: 210 - animationSpeed.value; running: playButton.checked; repeat: true
        onTriggered: nextFrame();
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
