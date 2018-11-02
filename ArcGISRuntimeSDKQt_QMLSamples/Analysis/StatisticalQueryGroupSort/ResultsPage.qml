
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

Rectangle {
    color: "#F4F4F4"
    signal backClicked()

    property var statisticResult

    Column {
        anchors.fill: parent
        clip: true

        Rectangle {
            id: titleBar
            width: parent.width
            height: 40 * scaleFactor
            color: "#005e95"
            clip: true

            Item {
                anchors {
                    left: parent.left
                    margins: 10 * scaleFactor
                    verticalCenter: parent.verticalCenter
                }
                height: 30 * scaleFactor
                width: 100 * scaleFactor

                MouseArea {
                    anchors.fill: parent
                    onClicked: backClicked()
                }

                Text {
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }

                    text: "< Options"
                    color: "white"
                    font {
                        pixelSize: 18 * scaleFactor
                        bold: true
                    }
                }
            }

            Text {
                anchors {
                    centerIn: parent
                    margins: 10 * scaleFactor
                }
                text: "Results"
                color: "white"
                font.pixelSize: 28 * scaleFactor
            }
        }

        Item {
            width: parent.width
            height: parent.height - titleBar.height

            ListView {
                id: resultView
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                model: statisticResult
                clip: true

                section {
                    property: "section"
                    delegate: Rectangle {
                        width: resultView.width
                        height: 25 * scaleFactor
                        color: "lightsteelblue"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: section
                            font.bold: true
                            font.pixelSize: 12 * scaleFactor
                        }
                    }
                }

                delegate: Item {
                    width: parent.width
                    height: 25 * scaleFactor

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: statistic
                        font.pixelSize: 12 * scaleFactor
                    }
                }
            }
        }
    }
}
