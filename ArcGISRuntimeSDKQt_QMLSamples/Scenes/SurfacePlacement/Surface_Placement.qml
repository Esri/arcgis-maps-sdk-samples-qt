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
import Esri.ArcGISRuntime 100.9

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

            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer"
            }

            // set an initial viewpoint
            ViewpointCenter {
                Point {
                    id: initialViewpoint
                    x: -4.45968
                    y: 48.3889
                    z: 37.9922
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                targetScale: 62.013264927081764

                Camera {
                    id: camera
                    location: initialViewpoint
                    heading: 329.91
                    pitch: 96.6632
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
                geometry: surfaceRelatedPoint

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: surfaceRelatedPoint

                TextSymbol {
                    size: 20
                    text: "DRAPED FLAT"
                    color: "blue"
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
                geometry: surfaceRelatedPoint

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: surfaceRelatedPoint

                TextSymbol {
                    size: 20
                    text: "DRAPED BILLBOARDED"
                    color: "blue"
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
                geometry: surfaceRelatedPoint

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: surfaceRelatedPoint

                TextSymbol {
                    size: 20
                    text: "RELATIVE"
                    color: "blue"
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
                geometry: surfaceRelatedPoint

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: surfaceRelatedPoint

                TextSymbol {
                    size: 20
                    text: "ABSOLUTE"
                    color: "blue"
                    horizontalAlignment: Enums.HorizontalAlignmentLeft
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: 20
                }
            }
        }

        GraphicsOverlay {
            id: relToScene
            sceneProperties.surfacePlacement: Enums.SurfacePlacementRelativeToScene

            // Graphics will be placed at absolute z value
            Graphic {
                geometry: sceneRelatedPoint

                SimpleMarkerSymbol {
                    color: "red"
                    style: Enums.SimpleMarkerSymbolStyleTriangle
                    size: 20
                }
            }

            Graphic {
                geometry: sceneRelatedPoint

                TextSymbol {
                    size: 20
                    text: "RELATIVE TO SCENE"
                    color: "blue"
                    horizontalAlignment: Enums.HorizontalAlignmentRight
                    verticalAlignment: Enums.VerticalAlignmentMiddle
                    offsetX: -20
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

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .8
        radius: 5

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ColumnLayout {
            Text {
                id: zValueSliderLabel
                text: qsTr("Z-Value")
                color: "white"
                Layout.margins: 2
                Layout.alignment: Qt.AlignHCenter
            }

            Slider {
                id: zValueSlider
                from: 0
                to: 140
                value: 70
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 2
                orientation: Qt.Vertical

                onMoved: changeZValue(value.toFixed(0));

                // Custom slider handle that displays the current value
                handle: Item {
                    x: parent.leftPadding + parent.availableWidth / 2 - headingHandleNub.width / 2
                    y: parent.topPadding + parent.visualPosition * (parent.availableHeight - headingHandleNub.height)

                    Rectangle {
                        id: headingHandleNub
                        color: headingHandleRect.color
                        radius: width * 0.5
                        width: 20
                        height: width
                    }
                    Rectangle {
                        id: headingHandleRect
                        height: childrenRect.height
                        width: childrenRect.width
                        radius: 3
                        x: headingHandleNub.x - width
                        y: headingHandleNub.y - height / 2 + headingHandleNub.height / 2
                        color: zValueSlider.background.children[0].color

                        Text {
                            id: headingValue
                            font.pixelSize: 14
                            padding: 3
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            text: (zValueSlider.value).toFixed(0)
                            color: "white"
                        }
                    }
                }
            }
        }
    }
    // create a point with a z value of 70
    Point {
        id: sceneRelatedPoint
        x: -4.4610562
        y: 48.3902727
        z: 70
        spatialReference: SpatialReference { wkid: 4326 }
    }
    // create a point with a z value of 70
    Point {
        id: surfaceRelatedPoint
        x: -4.4609257
        y: 48.3903965
        z: 70
        spatialReference: SpatialReference { wkid: 4326 }
    }

    function changeZValue(zValue) {
        sceneView.graphicsOverlays.forEach( function(overlay) {
            overlay.graphics.forEach( function(graphic) {
                // create new graphic with the same existing information but a new Z-value
                const graphicPoint = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                               x:graphic.geometry.x,
                                                                               y:graphic.geometry.y,
                                                                               z:zValue,
                                                                               spatialReference: graphic.geometry.spatialReference
                                                                           });
                graphic.geometry = graphicPoint;
            });
        });
    }
}
