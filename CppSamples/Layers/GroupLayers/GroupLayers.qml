// [WriteFile Name=GroupLayers, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
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
import Esri.Samples

Item {

    SceneView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on SceneView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    GroupLayersSample {
        id: sampleModel
        sceneView: view
    }

    // Create a window to display the layers
    Rectangle {
        id: layerVisibilityRect
        color: palette.base
        border.color: "darkgray"
        opacity: 1
        radius: 5
        width: 250
        height: layerVisibilityListView.contentHeight + 20


        // Create a list view to display the items
        ListView {
            id: layerVisibilityListView
            anchors {
                fill: parent
                margins: 10
            }
            //clip: true
            spacing: 12

            // Assign the model to the list model of layers
            model: sampleModel.layerListModel

            // Assign the delegate to the delegate created above
            delegate: Item {
                height: childrenRect.height

                // select the component based on the layer type
                // GroupLayer is LayerType int value of 22. See API doc for more details:
                // https://developers.arcgis.com/qt/layers/
                Loader {
                    sourceComponent: layerType === 22 ?
                                         groupLayerDelegate : layerDelegate
                }

                // Delegate for GroupLayers - Contains secondary repeater for sublayers
                Component {
                    id: groupLayerDelegate
                    Column {
                        spacing: 8
                        CheckBox {
                            id: parentBox
                            text: name
                            checked: layerVisible
                            onToggled: layerVisible = checked
                        }

                        Repeater {
                            model: sampleModel.getGroupLayerListModel(layerVisibilityListView.currentIndex)
                            delegate: RadioButton {
                                checked: index === 0
                                text: name
                                leftPadding: indicator.width
                                width: layerVisibilityRect.width - leftPadding
                                onCheckedChanged: layerVisible = checked
                                enabled: parentBox.checked
                            }
                        }
                    }
                }

                // Delegate for all other layers - standard Checkbox
                Component {
                    id: layerDelegate
                    CheckBox {
                        text: name
                        checked: layerVisible
                        onToggled: layerVisible = checked;
                    }
                }
            }
        }
    }
}
