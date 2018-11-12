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
import QtQuick.Layouts 1.3
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

Animate3DSymbolsSample {
    id: rootRectangle

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D"
    property bool following: followButton.checked

    missionFrame: progressSlider.value
    zoom: cameraDistance.value
    angle: cameraAngle.value

    onNextFrameRequested: {
        progressSlider.value = progressSlider.value + 1;
        if (progressSlider.value >= missionSize)
            progressSlider.value = 0;
    }

    Component.onCompleted: {
        missionList.currentIndex = 0;
    }

    SceneView {
        id: sceneView
        objectName: "sceneView"
        anchors.fill: parent

        Rectangle {
            id: mapFrame
            anchors {
                left:sceneView.left
                bottom: sceneView.attributionTop
                margins: 10
            }
            height: parent.height * 0.25
            width: parent.width * 0.3
            color: "black"
            clip: true

            MapView {
                id: mapView
                objectName: "mapView"
                anchors {
                    fill: parent
                    margins: 2
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: mouse.accepted
                    onWheel: wheel.accepted
                }
            }

            RowLayout {
                anchors {
                    left: parent.left
                    top: parent.top
                }
                spacing: 10

                Rectangle {
                    Layout.margins: 5
                    height: width
                    width: childrenRect.width
                    clip: true
                    radius: 5

                    opacity: 0.9
                    Image {
                        source: "qrc:/Samples/Scenes/Animate3DSymbols/plus-16-f.png"
                        width: 24
                        height: width

                        MouseArea {
                            anchors.fill: parent
                            onClicked: zoomMapIn()
                        }
                    }
                }

                Rectangle {
                    Layout.margins: 5
                    height: width
                    width: childrenRect.width
                    opacity: 0.9
                    clip: true
                    radius: 5

                    Image {
                        source: "qrc:/Samples/Scenes/Animate3DSymbols/minus-16-f.png"
                        width: 24
                        height: width
                        MouseArea {
                            anchors.fill: parent
                            onClicked: zoomMapOut()
                        }
                    }
                }
            }
        }
    }

    RowLayout {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }

        GroupBox {
            ColumnLayout {
                spacing: 10

                ComboBox {
                    id: missionList
                    enabled: !playButton.checked
                    model: missionsModel
                    textRole: "display"
                    property real modelWidth: 0
                    implicitWidth: leftPadding + rightPadding + indicator.width + modelWidth

                    onModelChanged: {
                        for (var i = 0; i < missionsModel.rowCount(); ++i) {
                            var index = missionsModel.index(i, 0);
                            textMetrics.text = missionsModel.data(index);
                            modelWidth = Math.max(modelWidth, textMetrics.width);
                        }
                        currentIndex = -1;
                    }

                    onCurrentTextChanged: {
                        changeMission(currentText);
                        progressSlider.value = 0;
                    }

                    TextMetrics {
                        id: textMetrics
                        font: missionList.font
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
                    text: "progress"
                    style: Text.Outline
                    styleColor: "white"
                    font.pixelSize: 14
                }

                Slider {
                    id: progressSlider
                    from: 0
                    to: missionSize
                    enabled : missionReady
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
            Layout.alignment: Qt.AlignRight

            ColumnLayout {
                spacing: 5
                layoutDirection: Qt.RightToLeft

                Text {
                    text: "zoom"
                    enabled: following && missionReady
                    style: Text.Outline
                    styleColor: "white"
                    font.pixelSize: 14
                }

                Slider {
                    id: cameraDistance
                    enabled: following && missionReady
                    from: 10.0
                    to: 5000.0
                    value: 500.0
                }

                Text {
                    text: "angle"
                    enabled: following && missionReady
                    style: Text.Outline
                    styleColor: "white"
                    font.pixelSize: 14
                }

                Slider {
                    id: cameraAngle
                    enabled: following && missionReady
                    from: 0.0
                    to: 180.0
                    value: 45.0
                }

                Text {
                    text: "speed"
                    enabled: missionReady
                    style: Text.Outline
                    styleColor: "white"
                    font.pixelSize: 14

                }

                Slider {
                    id: animationSpeed
                    enabled: missionReady
                    from: 1
                    to: 100
                    value: 50
                }
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
