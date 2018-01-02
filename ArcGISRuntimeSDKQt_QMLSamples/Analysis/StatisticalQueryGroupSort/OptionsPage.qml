
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
import Esri.ArcGISRuntime 100.2

Rectangle {
    id: rootOptionsPage
    color: "#F4F4F4"

    property real pageWidth: rootOptionsPage.width - flickable.anchors.margins * 2
    signal statisticButtonClicked()
    property var fields: null
    property var statisticTypes: ["Average", "Count", "Maximum", "Minimum", "Standard Deviation", "Sum", "Variance"]
    property var statisticsModel: [{field: "POP2007", statistic: "Sum"}, {field: "POP2007", statistic: "Average"}, {field: "AGE_5_17", statistic: "Minimum"}]
    property var groupingModel: [{field: "SUB_REGION", grouping: "Ascending"}]

    Flickable {
        id: flickable
        anchors {
            fill: parent
            margins: 10 * scaleFactor
        }

        contentWidth: column.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: column
            width: pageWidth
            spacing: 10 * scaleFactor

            Rectangle {
                width: pageWidth
                height: 250 * scaleFactor
                color: "transparent"
                clip: true
                border {
                    width: 1 * scaleFactor
                    color: "black"
                }

                Column {
                    anchors {
                        fill: parent
                        margins: 10 * scaleFactor
                    }
                    spacing: 5 * scaleFactor

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Statistics: US States"
                        font.underline: true
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 5 * scaleFactor

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Field"
                        }

                        ComboBox {
                            id: fieldComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            model: fields
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Type"
                        }

                        ComboBox {
                            id: statisticComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            model: statisticTypes
                        }

                        Button {
                            text: "+"
                            width: 25 * scaleFactor
                            onClicked: {
                                statisticsModel.push({field: fieldComboBox.currentText, statistic: statisticComboBox.currentText});
                                statisticsModelChanged();
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 150 * scaleFactor
                        color: "transparent"
                        clip: true
                        border {
                            color: "black"
                            width:  1 * scaleFactor
                        }

                        ListView {
                            id: statisticView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlightFollowsCurrentItem: true
                            model: statisticsModel
                            clip: true
                            highlight: Rectangle {
                                color: "cyan"
                                opacity: 0.9
                            }
                            highlightResizeVelocity: 100000

                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10 * scaleFactor
                                    Text {
                                        text: modelData.field
                                    }
                                    Text {
                                        text: "(%1)".arg(modelData.statistic)
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        statisticView.currentIndex = index;
                                    }
                                }
                            }
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Remove Statistic"
                        onClicked: {
                            statisticsModel.splice(statisticView.currentIndex, 1);
                            statisticsModelChanged();
                        }
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 200 * scaleFactor
                spacing: 5 * scaleFactor

                Column {
                    width: parent.width * 0.45 - parent.spacing
                    height: parent.height
                    spacing: 5 * scaleFactor

                    Text {
                        text: "Group Field(s):"
                    }

                    Rectangle {
                        width: parent.width
                        height: 175 * scaleFactor
                        color: "transparent"
                        border {
                            width: 1 * scaleFactor
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: fieldView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlight: Rectangle {
                                color: "cyan"
                                opacity: 0.9
                            }
                            highlightResizeVelocity: 100000

                            clip: true
                            model: fields
                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        fieldView.currentIndex = index;
                                    }
                                }
                            }
                        }
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.1 - parent.spacing
                    spacing: 5 * scaleFactor

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 25 * scaleFactor
                        text: ">>"
                        onClicked: {
                            groupingModel.push({field: statisticsModel[fieldView.currentIndex].field, grouping: "Ascending"});
                            groupingModelChanged();
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 25 * scaleFactor
                        text: "<<"
                        onClicked: {

                        }
                    }
                }

                Column {
                    width: parent.width * 0.45
                    height: parent.height
                    spacing: 5 * scaleFactor

                    Text {
                        text: "Order by Field:"
                    }

                    Rectangle {
                        width: parent.width
                        height: 175 * scaleFactor
                        color: "transparent"
                        border {
                            width: 1 * scaleFactor
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: groupingView
                            anchors {
                                fill: parent
                                margins: 5 * scaleFactor
                            }
                            highlight: Rectangle {
                                color: "cyan"
                                opacity: 0.9
                            }
                            highlightResizeVelocity: 100000

                            clip: true
                            model: groupingModel
                            delegate: Item {
                                width: parent.width
                                height: 25 * scaleFactor

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10 * scaleFactor
                                    Text {
                                        text: modelData.field
                                    }
                                    Text {
                                        text: "(%1)".arg(modelData.grouping)
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        statisticView.currentIndex = index;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 250 * scaleFactor
                height: 50 * scaleFactor
                text: "<u><b>Get Statistics</b></u>"
                onClicked: statisticButtonClicked()
            }
        }
    }
}
