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
            width: 300
            height: childrenRect.height
            color: "lightgrey"

            GridLayout {
                columns: 1
                rows: 2
                rowSpacing: 0

                RowLayout {
                    id: buttonRow
                    spacing: 10
                    width: parent.width

                    Button {
                        id: backButton
                        text: "<"
                        enabled: !sampleModel.isTopLevel
                        background: Rectangle {
                            color: listViewWindow.color
                        }
                        onClicked: {
                            sampleModel.displayPreviousLevel();
                        }
                        highlighted: pressed
                    }

                    Rectangle {
                        id: textRectangle
                        width: listViewWindow.width - backButton.width - buttonRow.spacing
                        height: backButton.height
                        color: listViewWindow.color
                        Text {
                            id: textLabel
                            text: sampleModel.labelText
                            width: textRectangle.width
                            wrapMode: Text.Wrap
                        }
                    }
                }

                RowLayout {
                    ListView {
                        id: myListView
                        height: contentHeight
                        width: 200
                        spacing: 0
                        model: sampleModel.levelNodeNames
                        delegate: Component {
                            Button {
                                text: modelData
                                width: listViewWindow.width
                                onClicked: {
                                    sampleModel.processSelectedNode(text);
                                }
                                highlighted: pressed
                            }
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
