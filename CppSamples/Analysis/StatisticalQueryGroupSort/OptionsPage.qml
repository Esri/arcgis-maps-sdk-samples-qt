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
import QtQuick.Controls

Rectangle {
    id: rootOptionsPage
    color: palette.base

    property real pageWidth: rootOptionsPage.width - flickable.anchors.margins * 2
    signal statisticButtonClicked()
    property var fields: rootRectangle.fields
    property var groupingFields: rootRectangle.groupingFields
    property var statisticTypes: rootRectangle.statisticTypes
    property var statisticsModel: rootRectangle.statisticDefinitions
    property var orderByModel: rootRectangle.orderBys
    property real labelTextSize: 12

    // Create a Title Bar for the Options Page
    Rectangle {
        id: titleBar
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        color: palette.highlight
        height: 40
        clip: true

        Label {
            anchors.centerIn: parent
            text: qsTr("Statistics: US States")
            color: "white"
            font.pixelSize: 28
        }
    }

    // Create a Flickable Column to hold the various options for configuring statistics
    Flickable {
        id: flickable
        anchors {
            top: titleBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        contentWidth: column.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Column {
            id: column
            width: pageWidth
            spacing: 10

            Rectangle {
                width: pageWidth
                height: upperRectColumn.childrenRect.height + 10
                color: "transparent"
                clip: true
                border {
                    width: 1
                    color: "black"
                }

                Column {
                    id: upperRectColumn
                    anchors {
                        fill: parent
                        margins: 10
                    }
                    spacing: 5

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 5

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Field")
                            font.pixelSize: labelTextSize
                        }

                        ComboBox {
                            id: fieldComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            property int modelWidth: 0
                            width: modelWidth + leftPadding + rightPadding
                            model: fields
                            onModelChanged: {
                                for (let i = 0; i < model.length; ++i) {
                                    metricsFieldComboBox.text = model[i];
                                    modelWidth = Math.max(modelWidth, metricsFieldComboBox.width);
                                }
                            }
                            TextMetrics {
                                id: metricsFieldComboBox
                                font: fieldComboBox.font
                            }
                        }

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("Type")
                            font.pixelSize: labelTextSize
                        }

                        ComboBox {
                            id: statisticComboBox
                            anchors.verticalCenter: parent.verticalCenter
                            model: statisticTypes
                            property int modelWidth: 0
                            width: modelWidth + leftPadding + rightPadding
                            Component.onCompleted : {
                                for (let i = 0; i < model.length; ++i) {
                                    metricsStatisticComboBox.text = model[i];
                                    modelWidth = Math.max(modelWidth, metricsStatisticComboBox.width);
                                }
                            }
                            TextMetrics {
                                id: metricsStatisticComboBox
                                font: statisticComboBox.font
                            }
                        }

                        Button {
                            text: "+"
                            onClicked: rootRectangle.addStatisticDefinition(fieldComboBox.currentText, statisticComboBox.currentText);
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 125
                        color: "transparent"
                        clip: true
                        border {
                            color: "black"
                            width:  1
                        }

                        ListView {
                            id: statisticView
                            anchors {
                                fill: parent
                                margins: 5
                            }
                            highlightFollowsCurrentItem: true
                            model: statisticsModel
                            clip: true
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000

                            delegate: Item {
                                width: parent.width
                                height: 25

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10
                                    Label {
                                        text: modelData.field
                                        font.pixelSize: labelTextSize
                                    }
                                    Label {
                                        text: "(%1)".arg(modelData.statistic)
                                        font.pixelSize: labelTextSize
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: statisticView.currentIndex = index;
                                }
                            }
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Remove Statistic")
                        onClicked: rootRectangle.removeStatisticDefinition(statisticView.currentIndex);
                    }

                    // Spacer
                    Item {
                        width: parent.width
                        height: 2
                    }
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: 175
                spacing: 5

                Column {
                    width: parent.width * 0.45 - parent.spacing
                    height: parent.height
                    spacing: 5

                    Label {
                        text: qsTr("Group Field(s):")
                        font.pixelSize: labelTextSize
                    }

                    Rectangle {
                        width: parent.width
                        height: 125
                        color: "transparent"
                        border {
                            width: 1
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: fieldView
                            anchors {
                                fill: parent
                                margins: 5
                            }
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000
                            clip: true
                            model: fields
                            delegate: Item {
                                height: 25

                                CheckBox {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: modelData
                                    checked: text === "State"

                                    onCheckedChanged: {
                                        if (checked)
                                            rootRectangle.addGroupingField(text);
                                        else
                                            rootRectangle.removeGroupingField(text);
                                    }

                                    onClicked: fieldView.currentIndex = index;
                                }
                            }
                        }
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * 0.1 - parent.spacing
                    spacing: 5

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: ">"
                        onClicked: {
                            // return if the field is not selected
                            if (groupingFields.indexOf(fields[fieldView.currentIndex]) === -1)
                                return;

                            // add the data to the model
                            rootRectangle.addOrderBy(fields[fieldView.currentIndex], "Ascending")
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "<"
                        onClicked: rootRectangle.removeOrderBy(groupingView.currentIndex);
                    }
                }

                Column {
                    width: parent.width * 0.45
                    height: parent.height
                    spacing: 5

                    Label {
                        text: "Order by Field:"
                        font.pixelSize: labelTextSize
                    }

                    Rectangle {
                        width: parent.width
                        height: 100
                        color: "transparent"
                        border {
                            width: 1
                            color: "black"
                        }
                        clip: true

                        ListView {
                            id: groupingView
                            anchors {
                                fill: parent
                                margins: 5
                            }
                            highlight: highlightRectangle
                            highlightResizeVelocity: 1000000
                            highlightMoveVelocity: 1000000

                            clip: true
                            model: orderByModel
                            delegate: Item {
                                width: parent.width
                                height: 25

                                Row {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 10
                                    Label {
                                        text: modelData.field
                                        font.pixelSize: labelTextSize
                                    }
                                    Label {
                                        text: "(%1)".arg(modelData.order)
                                        font.pixelSize: labelTextSize
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: groupingView.currentIndex = index;
                                }
                            }
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Change Sort Order")
                        onClicked: rootRectangle.updateOrder(groupingView.currentIndex);
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 250
                height: 50
                text: qsTr("<u><b>Get Statistics</b></u>")
                onClicked: statisticButtonClicked()
            }
        }
    }

    Component {
        id: highlightRectangle
        Rectangle {
            color: palette.highlight
            opacity: 0.4
        }
    }
}
