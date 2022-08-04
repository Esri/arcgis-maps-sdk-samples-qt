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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.2
import Qt.labs.platform 1.1
import Esri.Samples 1.0
//import Esri.ArcGISExtras 1.1

Item {
    id: rootItem
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }




    CreateMobileGeodatabaseSample {
        id: model
        mapView: view
    }

    Rectangle {
        id: rectangle
        width: 250
        height: 400
        color: "#ffffff"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 10

        // Prevent mouse interaction from propagating to the MapView
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        Column {
            id: column1
            anchors.fill: parent
            anchors.rightMargin: 10
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
            anchors.topMargin: 10
            spacing: 5

            Button {
                id: createGdbButton
                text: qsTr("Create new .geodatabase")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: !model.gdbOpen
                onClicked: {
                    model.createGeodatabase();
                }
            }

            Button {
                id: viewGdbTableButton
                text: qsTr("View feature table")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: model.gdbOpen
                onClicked: {
                    gridRectangle.visible = true;
                    rectangle.visible = false;
                }
            }
            Button {
                id: clearFeaturesButton
                text: qsTr("Clear features")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: model.gdbOpen
                onClicked: {
                    model.clearTable();
                }
            }


            Button {
                id: closeGdbButton
                text: qsTr("Close .geodatabase")
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                enabled: model.gdbOpen
                onClicked: {
                    model.closeGdb();
                }
            }

            Text {
                id: text1
                text: model.gdbFilePath ? model.gdbFilePath.split("/").pop() : ""
                //elide: Text.ElideLeft
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 12
                wrapMode: Text.WrapAnywhere
                anchors.rightMargin: 0
                anchors.leftMargin: 0
            }

            Text {
                id: text2
                text: "Number of features: " + model.featureCount
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: 12
                wrapMode: Text.WordWrap
                anchors.rightMargin: 0
                anchors.leftMargin: 0
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    Rectangle {
        id: gridRectangle
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
            onPressed: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        ListView {
            id: tableView
            anchors.fill: parent
            anchors.margins: 10
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

    Button {
        id: closeTableButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.rightMargin: 10

        text: "Close table"
        visible: gridRectangle.visible
        onClicked: {
            gridRectangle.visible = false;
            rectangle.visible = true;
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
