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
import QtQuick.Controls 2.5
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                // add an arcgis tiled elevation source
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            //  Layers nested as children are appended to the group layer
            GroupLayer {
                id: gl
                name: "Buildings group"

                ArcGISSceneLayer {
                    url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_BuildingShells/SceneServer"
                }

                ArcGISSceneLayer {
                    url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevB_BuildingShells/SceneServer"
                }
            }

            // Add layers outside group layer
            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_Trees/SceneServer"
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/DevelopmentProjectArea/FeatureServer/0"
                }
            }

            FeatureLayer {
                ServiceFeatureTable {
                    url: "https://services.arcgis.com/P3ePLMYs2RVChkJx/arcgis/rest/services/DevA_Pathways/FeatureServer/1"
                }
            }
        }

        Component.onCompleted: {
            // create initial viewpoint extent
            const env = ArcGISRuntimeEnvironment.createObject("Envelope", {
                                                                  json: {
                                                                      "spatialReference": {
                                                                          "wkid":4326
                                                                      },
                                                                      "xmax":-122.67960721754773,
                                                                      "xmin":-122.68647066116789,
                                                                      "ymax":45.53584958588318,
                                                                      "ymin":45.531539857343745
                                                                  }
                                                              });

            // set viewpoint
            sceneView.setViewpoint(ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {
                                                                             extent: env
                                                                         }));
        }
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
        model: scene.operationalLayers

        // Assign the delegate to the delegate created above
        delegate: Item {
            property var type: layerType
            height: childrenRect.height

            // select the component based on the layer type
            Loader {
                sourceComponent: layerType === Enums.LayerTypeGroupLayer ?
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
                        property var groupLyr: scene.operationalLayers.get(layerVisibilityListView.currentIndex)
                        model: groupLyr ? groupLyr.layers : null
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
                    onCheckedChanged: layerVisible = checked
                }
            }
        }
    }
}
