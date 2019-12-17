// [WriteFile Name=Surface_Placement, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.ArcGISRuntime 100.8

Rectangle {
    width: 800
    height: 600    

    // Add a scene view
    SceneView {
        id: sceneView
        anchors.fill: parent

        // Add a scene with the imagery basemap and world elevation surface
        Scene {
            BasemapImagery {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    x: -4.04
                    y: 53.04
                    z: 1300
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 62.013264927081764

                Camera {
                    id: camera
                    location: Point {
                        x: -4.04
                        y: 53.04
                        z: 1300
                        spatialReference: SpatialReference { wkid: 4326 }
                    }
                    heading: 0
                    pitch: 90.0
                    roll: 0
                }
            }
        }

        // Graphics overlay with draped flat surface placement
        GraphicsOverlay {
            id: drapedFlatOverlay
            sceneProperties.surfacePlacement: Enums.SurfacePlacementDrapedFlat
            visible: false

            // Graphics will be draped on the surface of the scene and will appear flat
            Graphic {
                geometry: point

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: point

                TextSymbol {
                    size: 20
                    text: "DRAPED FLAT"
                    color: "white"
                    horizontalAlignment: Enums.HorizontalAlignmentLeft
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: 20
                }
            }
        }

        // Graphics overlay with draped billboard surface placement
        GraphicsOverlay {
            id: drapedBillboardedOverlay
            sceneProperties.surfacePlacement: Enums.SurfacePlacementDrapedBillboarded

            // Graphics will be draped on the surface of the scene and will always face the camera.
            Graphic {
                geometry: point

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: point

                TextSymbol {
                    size: 20
                    text: "DRAPED BILLBOARDED"
                    color: "white"
                    horizontalAlignment: Enums.HorizontalAlignmentLeft
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: 20
                }
            }
        }

        // Graphics overlay with relative surface placement
        GraphicsOverlay {
            id: relativeOverlay
            sceneProperties.surfacePlacement: Enums.SurfacePlacementRelative

            // Graphics will be placed at z value relative to the surface
            Graphic {
                geometry: point

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: point

                TextSymbol {
                    size: 20
                    text: "RELATIVE"
                    color: "white"
                    horizontalAlignment: Enums.HorizontalAlignmentLeft
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: 20
                }
            }
        }

        // Graphics overlay with absolute surface placement
        GraphicsOverlay {
            id: absoluteOverlay
            sceneProperties.surfacePlacement: Enums.SurfacePlacementAbsolute

            // Graphics will be placed at absolute z value
            Graphic {
                geometry: point

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: point

                TextSymbol {
                    size: 20
                    text: "ABSOLUTE"
                    color: "white"
                    horizontalAlignment: Enums.HorizontalAlignmentLeft
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: 20
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            margins: 3
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        ColumnLayout {
            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
                text: qsTr("Draped mode")
                color: "white"
            }

            Switch {
                id: surfacePlacementMode
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2

                onCheckedChanged: {
                    drapedFlatOverlay.visible = !drapedFlatOverlay.visible;
                    drapedBillboardedOverlay.visible = !drapedBillboardedOverlay.visible;
                }
            }
        }
    }

    // create a point with a z value of 1000
    Point {
        id: point
        x: -4.04
        y: 53.06
        z: 1000
        spatialReference: SpatialReference { wkid: 4326 }
    }
}
