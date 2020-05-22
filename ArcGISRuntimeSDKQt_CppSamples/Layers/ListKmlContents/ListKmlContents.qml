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
            //            Layout.margins: 3
            color: "lightgrey"

            ColumnLayout {

                RowLayout {
                    id: buttonRow
                    spacing: 10
                    width: parent.width

                    Button {
                        text: "<"
                        enabled: !sampleModel.isTopLevel
                        onClicked: {
                            sampleModel.displayPreviousLevel();
                        }
                    }
                }

                RowLayout {
                    ListView {
                        id: myListView
//                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 400
                        width: 200
                        spacing: 0
                        model: sampleModel.levelNodeNames
//                        onModelChanged: {
//                            if (modelData.length < 2) {
//                                console.log("< 2 elements");
//                            }
//                        }

                        delegate: Component {
                            Button {
                                text: modelData
                                width: listViewWindow.width
                                onClicked: {
                                    sampleModel.nodeSelected(text);
                                }
                            }
                        }
                    }
//                    StackView {
//                        id: stackView
//                        width: parent.width
//                        onDepthChanged: console.log("number of layers: ", depth);
//                    }
                }
            }
        }
    }

//    Component {
//        id: mapSelectViewComponent

//        Item {
//            id: mapSelectView

//            Column {
//                anchors {
//                    top: parent.top
//                    left: parent.left
//                }
//                width: parent.width
//                spacing: 20

//                // UI navigation bar
//                //            Rectangle {
//                //                width: parent.width
//                //                height: 100
//                //                color: "#283593"
//                //            }

//                ListView {
//                    id: myListView
//                    anchors.horizontalCenter: parent.horizontalCenter
//                    height: 400
//                    width: 200
//                    spacing: 0
//                    model: sampleModel.levelNodeNames

//                    delegate: Component {
//                        Button {
//                            text: modelData
//                            width: listViewWindow.width
//                            onClicked: {
//                                sampleModel.nodeSelected(text);
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }


    Connections {
        target: sampleModel
//        onLevelNodeNamesChanged: {
//            stackView.push(mapSelectViewComponent);
//        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ListKmlContentsSample {
        id: sampleModel
        sceneView: view
    }
}
