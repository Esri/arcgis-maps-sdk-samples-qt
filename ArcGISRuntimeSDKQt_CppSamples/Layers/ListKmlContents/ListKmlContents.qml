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

            ListView {
                id: listModel
                model: sampleModel.nodeNames
                height: contentHeight
                width: 200
                delegate: Text {
                    text: modelData
                    height: modelData.height
                }
                Component.onCompleted: {
                    console.log(sampleModel.nodeNames.length);
                }

//                Connections {
//                 target: sampleModel
//                 onNodeNamesChanged: console.log(sampleModel.nodeNames.length);
//                }
            }
        }
        //        StackView {
        //            id: stackView
        //        }
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
