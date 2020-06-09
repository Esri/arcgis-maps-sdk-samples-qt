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
        Control {
            width: 300
            background : Rectangle {
                color: "lightgrey"
            }
            contentItem: GridLayout {
                columns: 2
                Button {
                    id: backButton
                    Layout.margins: 3
                    text: "<"
                    enabled: !sampleModel.isTopLevel
                    flat: true
                    highlighted: pressed
                    onClicked: {
                        sampleModel.displayPreviousLevel();
                    }
                }
                Text {
                    Layout.fillWidth: true
                    id: textLabel
                    text: sampleModel.labelText
                    wrapMode: Text.Wrap
                }
                ListView {
                    id: myListView
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    model: sampleModel.levelNodeNames
                    Layout.preferredHeight: contentHeight
                    delegate: Button {
                        text: modelData
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        highlighted: pressed
                        onClicked: {
                            sampleModel.processSelectedNode(text);
                        }
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ListKmlContentsSample {
        id: sampleModel
        sceneView: view
    }
}
