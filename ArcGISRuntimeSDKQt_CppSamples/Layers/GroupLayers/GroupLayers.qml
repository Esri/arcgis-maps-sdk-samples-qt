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
            fill: layerVisibilityListView
            margins: -5
        }
        color: "lightgrey"
        border.color: "#4D4D4D"
        opacity: 0.9
        radius: 5
    }

    // Create a list view to display the items
    ListView {
        id: layerVisibilityListView
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        width: 250
        height: childrenRect.height
        clip: true

        // Assign the model to the list model of layers
        model: sampleModel.layerListModel

        // Assign the delegate to the delegate created above
        delegate: Item {
            height: childrenRect.height

            // select the component based on the layer type
            // GroupLayer is LayerType int value of 22. See API doc for more details:
            // https://developers.arcgis.com/qt/latest/cpp/api-reference/esri-arcgisruntime-layertype.html
            Loader {
                sourceComponent: layerType === 22 ?
                                     groupLayerDelegate : layerDelegate
            }

            // Delegate for GroupLayers - Contains secondary repeater for sublayers
            Component {
                id: groupLayerDelegate
                Column {
                    CheckBox {
                        id: parentBox
                        text: name
                        checked: true
                        onCheckedChanged: layerVisible = checked
                    }

                    Repeater {
                        model: sampleModel.getGroupLayerListModel(layerVisibilityListView.currentIndex)
                        delegate: CheckBox {
                            checked: true
                            text: name
                            leftPadding: indicator.width
                            width: layerVisibilityRect.width - leftPadding
                            onCheckedChanged: layerVisible = checked
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
                        layerVisible = checked;
                    }
                }
            }
        }
    }
}
