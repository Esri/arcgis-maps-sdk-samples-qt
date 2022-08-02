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
import Esri.Samples 1.0

Item {
    id: item1

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        id: gridRectangle
        width: parent.width - parent.width * 0.2
        height: parent.height - parent.height * 0.2
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#3168ba"

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
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.2
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "featureOidRole"

                    }
                }
                Rectangle {
                    color: "white"
                    border.color: "black"
                    width: parent.width * 0.8
                    height: parent.height
                    Text {
                        anchors.centerIn: parent
                        text: "featureTimestampRole"
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
        id: tableToggle
        text: "toggle table"
        onClicked: {
            gridRectangle.visible = !gridRectangle.visible;
        }
    }
    Button {
        id: clearFeatures
        anchors.top: tableToggle.bottom
        text: "clear features"
        onClicked: {
            model.clearTable();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    CreateMobileGeodatabaseSample {
        id: model
        mapView: view
    }
}
