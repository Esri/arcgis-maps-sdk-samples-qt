// [Legal]
// Copyright 2022 Esri.

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
import Esri.ArcGISRuntimeSamples 1.0

Item {
    visible: false

    RadialGradient {
        id: overlay
        anchors.fill: parent
        opacity: 0.7
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgrey" }
            GradientStop { position: 0.7; color: "black" }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: aboutView.visible = false
        }
    }

    Page {
        id: aboutPage
        anchors.centerIn: parent
        width: 265
        height: 240
        clip: true
        background: Rectangle {
            radius: 3
        }

        SwipeView {
            id: view
            anchors.fill: parent
            currentIndex: 0
            clip: true
            background: Rectangle {
                radius: 3
            }

            Page {
                clip: true
                background: Rectangle {
                    radius: 3
                }

                Column {
                    anchors {
                        fill: parent
                        margins: 10
                        topMargin: 30
                    }
                    spacing: 15

                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "qrc:/esri_logo.png"
                        width: 210
                        height: 66
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("ArcGIS Runtime SDK for Qt Samples")
                        font {
                            family: fontFamily
                            pixelSize: 14
                            weight: Font.DemiBold
                            italic: true
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Built with ArcGIS Runtime SDK for Qt")
                        font {
                            family: fontFamily
                            pixelSize: 13
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: buildNum // obtained from main.cpp (engine.rootContext()->setContextProperty)
                        font {
                            family: fontFamily
                            pixelSize: 13
                        }
                    }
                }
            }

            Page {
                clip: true
                background: Rectangle {
                    radius: 3
                }

                Column {
                    anchors {
                        fill: parent
                        margins: 10
                        topMargin: 30
                    }
                    spacing: 15

                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "qrc:/logo.png"
                        width: 72
                        height: 72
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("View the samples on GitHub")
                        renderType: Text.NativeRendering
                        font {
                            family: fontFamily
                            pixelSize: 13
                        }
                        color: "blue"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: Qt.openUrlExternally(SampleManager.qtSamplesUrl)
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Learn more about the SDK")
                        renderType: Text.NativeRendering
                        font {
                            family: fontFamily
                            pixelSize: 13
                        }
                        color: "blue"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: Qt.openUrlExternally(SampleManager.qtSdkUrl)
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Built with Qt %1".arg(qtVersion)) // obtained from main.cpp (engine.rootContext()->setContextProperty)
                        renderType: Text.NativeRendering
                        font {
                            family: fontFamily
                            pixelSize: 13
                        }
                    }
                }
            }
        }
    }

    Image {
        anchors {
            left: aboutPage.left
            bottom: aboutPage.bottom
            margins: 10
        }
        source: "qrc:/Built_with_Qt_RGB_logo.png"
        width: 75
        height: 18

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.openUrlExternally("https://links.esri.com/qtIO")
        }
    }

    Label {
        anchors {
            right: aboutPage.right
            bottom: aboutPage.bottom
            margins: 10
        }
        text: qsTr("CLOSE")
        font {
            family: fontFamily
            pixelSize: 13
        }
        color: "#005e95"

        MouseArea {
            anchors.fill: parent
            onClicked: aboutView.visible = false
        }
    }

    PageIndicator {
        id: indicator
        anchors {
            bottom: aboutPage.bottom
            horizontalCenter: aboutPage.horizontalCenter
        }
        count: view.count
        currentIndex: view.currentIndex

        delegate: Rectangle {
            implicitWidth: 6
            implicitHeight: 6
            radius: width / 2
            color: "black"
            opacity: index === indicator.currentIndex ? 0.95 : pressed ? 0.7 : 0.45

            Behavior on opacity {
                OpacityAnimator {
                    duration: 100
                }
            }
        }
    }
}
