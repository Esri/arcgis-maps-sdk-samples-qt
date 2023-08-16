// [WriteFile Name=CreateMobileGeodatabase, Category=Features]
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import Esri.Samples

Item {
    id: rootItem
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    CreateMobileGeodatabaseSample {
        id: model
        mapView: view
    }

    Rectangle {
        id: buttonListRectangle
        anchors {
            top: parent.top
            right: parent.right
            topMargin: 10
            rightMargin: 10
        }

        width: 250
        height: buttonColumn.height + 20
        color: "#ffffff"

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        Column {
            id: buttonColumn
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                topMargin: 10
                leftMargin: 10
                rightMargin: 10
            }
            height: children.height
            spacing: 5

            onHeightChanged: {
                buttonListRectangle.height = height + 20
            }

            Button {
                id: createGdbButton
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: qsTr("Create new .geodatabase")
                enabled: !model.gdbOpen
                onClicked: {
                    model.createGeodatabase();
                }
            }

            Button {
                id: viewGdbTableButton
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: qsTr("View feature table")
                enabled: model.gdbOpen
                onClicked: {
                    featureTableDisplay.visible = true;
                    buttonListRectangle.visible = false;
                }
            }
            Button {
                id: clearFeaturesButton
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: qsTr("Clear features")
                enabled: model.gdbOpen
                onClicked: {
                    model.clearFeatures();
                }
            }


            Button {
                id: closeGdbButton
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: qsTr("Close .geodatabase")
                enabled: model.gdbOpen
                onClicked: {
                    model.closeGdb();
                    gdbClosedNoticeRectangle.visible = true;
                    buttonListRectangle.visible = false;
                }
            }

            Text {
                id: fileNameText
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: model.gdbFilePath ? "Created new geodatabase:\n" + model.gdbFilePath.split("/").pop() : "Geodatabase path not found"
                font.pixelSize: 12
                wrapMode: Text.WrapAnywhere
                visible: model.gdbOpen
            }

            Text {
                id: featureCountText
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 0
                    rightMargin: 0
                }
                text: "Number of features: " + model.featureCount + (model.featureCount === 0 ? "\n(Click or tap on the map to add new features)" : "")
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                visible: model.gdbOpen
            }
        }
    }

    Rectangle {
        id: gdbClosedNoticeRectangle
        anchors.centerIn: parent
        width: parent.width * 0.75
        height: gdbInfoColumn.height + 20
        color: "white"
        border.color: "black"
        clip: true
        visible: false

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        Column {
            id: gdbInfoColumn
            anchors {
                centerIn: parent
                margins: 10
            }
            spacing: 10
            width: parent.width - 20
            height: children.height

            Text {
                id: gdbNameText
                width: parent.width
                text: "Closed and saved geodatabase to the temporary path:"
                wrapMode: Text.WordWrap
            }
            TextEdit {
                id: gdbPathText
                width: parent.width
                readOnly: true
<<<<<<< HEAD
                activeFocusOnPress: false
=======
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
                selectByMouse: true
                text: model.gdbFilePath
                wrapMode: Text.WrapAnywhere
            }
            Button {
                id: gdbInfoClose
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Ok"
                onClicked: {
                    gdbClosedNoticeRectangle.visible = false;
                    buttonListRectangle.visible = true;
                }
            }
        }
    }

    Rectangle {
        id: featureTableDisplay
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - parent.width * 0.2
        height: parent.height - parent.height * 0.3
        color: "#80808080"
        visible: false

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        ListView {
            id: tableView
            anchors {
                fill: parent
                margins: 10
            }
            ScrollBar.vertical: ScrollBar {
                active: true
            }
            clip: true

            header: Row {
                height: 40
                width: tableView.width
                Rectangle {
                    color: "grey"
                    border.color: "black"
                    width: parent.width * 0.2
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "Object ID"
                        color: "white"
                        font.bold: true

                    }
                }
                Rectangle {
                    color: "grey"
                    border.color: "black"
                    width: parent.width * 0.8
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "Timestamp"
                        color: "white"
                        font.bold: true
                    }
                }
            }

            model: model.featureListModel
            delegate: Row {
                height: 40
                width: tableView.width
                Rectangle {
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.2
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: featureOidRole

                    }
                }
                Rectangle {
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.8
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: featureTimestampRole
                    }
                }
            }
        }
    }

    Rectangle {
        id: closeTableButtonRectangle
        anchors {
            top: parent.top
            right: parent.right
            topMargin: 10
            rightMargin: 10
        }
        width: closeTableButton.width + 10
        height: closeTableButton.height + 10
        color: "#ffffff"
        visible: featureTableDisplay.visible

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        Button {
            id: closeTableButton
            anchors.centerIn: parent

            text: "Close table view"

            onClicked: {
                featureTableDisplay.visible = false;
                buttonListRectangle.visible = true;
            }
        }
    }
}
