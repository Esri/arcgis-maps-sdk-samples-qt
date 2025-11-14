// [WriteFile Name=ListKmlContents, Category=Layers]
// [Legal]
// Copyright 2020 Esri.
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
import QtQuick.Layouts
import Esri.Samples

Item {

    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Create window for displaying the KML contents
        Control {
            width: 300
            padding: 2
            background : Rectangle {
                color: palette.base
                border{
                    color: palette.dark
                    width: 1
                }
            }
            contentItem: GridLayout {
                columns: 2
                columnSpacing: 2
                rowSpacing: 2
                Button {
                    id: backButton
                    text: "<"
                    enabled: !sampleModel.isTopLevel
                    flat: true
                    highlighted: pressed
                    onClicked: {
                        sampleModel.displayPreviousLevel();
                    }

                    Binding {
                        target: backButton.contentItem
                        property: "color"
                        value: Qt.styleHints.colorScheme === Qt.Dark ? "white" : "black"
                    }
                }
                Label {
                    Layout.fillWidth: true
                    id: textLabel
                    text: sampleModel.labelText
                    elide: Text.ElideRight
                }
                ListView {
                    id: myListView
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    model: sampleModel.levelNodeNames
                    Layout.preferredHeight: contentHeight
                    spacing:4
                    delegate: Button {
                        text: modelData
                        width: myListView.width
                        highlighted: pressed
                        onClicked: {
                            sampleModel.processSelectedNode(text);
                        }
                    }
                }
                // Spacer
                Item {
                    width: parent.fillWidth
                    height: 4
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
