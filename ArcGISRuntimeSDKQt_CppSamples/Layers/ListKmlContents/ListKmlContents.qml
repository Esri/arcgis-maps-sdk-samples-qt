// [WriteFile Name=ListKmlContents, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Layouts 1.11
import Esri.Samples 1.0

Item {

    property var nodeNamesList: []

    SceneView {
        id: view
        anchors.fill: parent

        // Create window for displaying the KML contents
        Rectangle {
            id: listViewWindow
            visible: true
            width: 200
            height: 200
            Layout.margins: 3
            color: "lightgrey"

            StackView {
                id: stackView
                anchors.fill: parent

//                initialItem: hierarchyItem

            }

//            ListView {
//                id: listModel
//                model: sampleModel.nodeNames
//                height: contentHeight
//                width: 200
//                delegate: Text {
//                    text: modelData
//                    height: modelData.height
//                }
//            }
        }
    }

    Item {
        id: hierarchyItem

        Rectangle {
            visible: true
            width: 200
            height: 200
            Layout.margins: 3
            color: "lightgrey"

            ListView {
                id: listView
                model: sampleModel.nodesList
                onModelChanged: {
                    console.log(sampleModel.nodesList.columnCount())
                }
            }
        }
    }

    Item {
        id: mapSelectView

        Column {
            anchors {
                top: parent.top
                left: parent.left
            }
            width: parent.width
            spacing: 20

            // UI navigation bar
            Rectangle {
                width: parent.width
                height: 100
                color: "#283593"
            }

            // list of maps
            ListView {
                anchors.horizontalCenter: parent.horizontalCenter
                height: 400
                width: 200
                spacing: 10
                model: sampleModel.nodeNames
//                model: mobileMapSearchRoute.mapList

                delegate: Component {
                    Text {
                        text: contentData
                    }
                    Rectangle {
                        width: 200
                        height: 50
                        color: "#283593"
                        radius: 2
                        border.color: "darkgray"

                    }
                }
            }
        }
    }


    Connections {
        target: sampleModel
//        onLevelAddedChanged: {
//            console.log("new level added");
//        }
        onNodesListChanged: {
            if (sampleModel.nodesList === null) {
                return;
            }
            nodeNamesList = [];
//            for (let i = 0; i < sampleModel.nodesList.rowCount(); i++) {
//                console.log(sampleModel.nodesList.index())
//            }
            console.log(sampleModel.nodesList.rowCount());
        }
    }

    Component {
        id: listViewDelegate
        Text {

        }
//        Rectangle {
//            id: rect
//            width: parent.width
//            height: 35
//            color: "white"


//        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ListKmlContentsSample {
        id: sampleModel
        sceneView: view
    }
}
