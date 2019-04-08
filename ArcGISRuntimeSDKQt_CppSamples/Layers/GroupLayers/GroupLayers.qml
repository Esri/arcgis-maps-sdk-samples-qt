// [WriteFile Name=GroupLayers, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import Esri.Samples 1.0

Item {

    SceneView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    GroupLayersSample {
        id: sampleModel
        sceneView: view
    }

    // Create a window to display the layers
    Rectangle {
        id: layerVisibilityRect
        anchors {
            margins: 5
            left: parent.left
            top: parent.top
        }
        height: 250
        width: 250
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.fill: parent
            width: layerVisibilityRect.width
            height: layerVisibilityRect.height
            color: "lightgrey"
            opacity: .9
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            // Create a list view to display the items
            ListView {
                id: layerVisibilityListView
                anchors.margins: 10
                width: parent.width
                height: parent.height
                clip: true

                // Assign the model to the list model of layers
                model: sampleModel.layerListModel

                // Assign the delegate to the delegate created above
                delegate: Item {
                    height: childrenRect.height

                    // select the component based on the layer type
                    Loader {
                        sourceComponent: layerType === 22 ?
                                             groupLayerDelegate : layerDelegate
                    }

                    // Delegate for GroupLayers - Contains secondary repeater for sublayers
                    Component {
                        id: groupLayerDelegate
                        Column {
                            ButtonGroup {
                                id: childGroup
                                exclusive: false
                                checkState: parentBox.checkState
                            }

                            CheckBox {
                                id: parentBox
                                text: name
                                checkState: childGroup.checkState
                            }

                            Repeater {
                                model: sampleModel.getGroupLayerListModel(layerVisibilityListView.currentIndex)
                                delegate: CheckBox {
                                    checked: true
                                    text: name
                                    leftPadding: indicator.width
                                    width: layerVisibilityRect.width - leftPadding
                                    ButtonGroup.group: childGroup
                                    onCheckedChanged: {
                                        layerVisible = checked
                                        console.log(name, layerVisible)
                                    }
                                }
                            }
                        }
                    }

                    // Delegate for all other layers - standard Checkbox
                    Component {
                        id: layerDelegate
                        CheckBox {
                            text: name
                            checked: true
                            onCheckedChanged: {
                                layerVisible = checked
                                console.log(name, layerVisible)
                            }
                        }
                    }
                }
            }
        }
    }
}
