// [WriteFile Name=ViewshedLocation, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime 100.5

Rectangle {
    id: viewshedSample
    clip: true
    width: 800
    height: 600

    
    property bool calculating: false

    SceneView {
        id: sceneView
        anchors.fill: parent
        focus: true

        Scene {
            id: scene
            BasemapTopographic {}

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    // Set a viewpoint
                    var point = ArcGISRuntimeEnvironment.createObject("Point", {
                                                                          x: 6.86088,
                                                                          y: 45.3604,
                                                                          z: 3582.55,
                                                                          spatialReference: SpatialReference.createWgs84()
                                                                      });

                    var camera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                           location: point,
                                                                           heading: 345,
                                                                           pitch: 70,
                                                                           roll: 0
                                                                       });

                    sceneView.setViewpointCamera(camera)
                }
            }
        }

        // Add an Analysis Overlay
        AnalysisOverlay {
            id: analysisOverlay

            LocationViewshed {
                id: locationViewshed
                minDistance: 50
                maxDistance: 1000
                horizontalAngle: 45
                verticalAngle: 90
                heading: 180
                pitch: 90
                visible: true
            }
        }

        onMouseClicked: locationViewshed.location = sceneView.screenToBaseSurface(mouse.x, mouse.y);
        onMousePressedAndHeld: calculating = true;
        onMouseReleased: calculating = false;
        onMousePositionChanged: locationViewshed.location = sceneView.screenToBaseSurface(mouse.x, mouse.y);

        Rectangle {
            anchors {
                right: parent.right
                bottom: sceneView.attributionTop
                margins: 10
            }
            visible: !optionPanel.visible
            width: 45
            height: width
            color: "white"
            radius: 25

            Image {
                anchors.centerIn: parent
                source: "qrc:/Samples/Analysis/ViewshedLocation/settings.png"
                width: 40
                height: width
            }

            MouseArea {
                anchors.fill: parent
                onClicked: optionPanel.visible = true;
            }
        }


        Rectangle {
            id: optionPanel
            anchors {
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
            }
            width: 260
            visible: false
            color: "white"
            opacity: 0.85

            Flickable {
                anchors {
                    fill: parent
                    margins: 5
                }
                contentWidth: parent.width
                contentHeight: parent.height
                flickableDirection: Flickable.VerticalFlick

                Column {
                    id: optionColumn
                    spacing: 10
                    width: optionPanel.width

                    Item {
                        width: parent.width
                        height: 25

                        Text {
                            text: "Viewshed Options"
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pixelSize: 18
                            font.underline: true

                        }

                        Rectangle {
                            anchors {
                                right: parent.right
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            width: 45
                            height: width
                            color: "transparent"

                            Image {
                                anchors.centerIn: parent
                                source: "qrc:/Samples/Analysis/ViewshedLocation/close.png"
                                width: 40
                                height: width
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: optionPanel.visible = false;
                            }
                        }
                    }

                    Item {
                        width: parent.width
                        height: 25

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.75
                            text: qsTr("Viewshed Visible")
                            font.pixelSize: 14
                        }

                        Switch {
                            anchors {
                                right: parent.right
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            checked: true
                            onCheckedChanged: locationViewshed.visible = checked;
                        }
                    }

                    Item {
                        width: parent.width
                        height: 25

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.75
                            text: qsTr("Frustum Outline Visible")
                            font.pixelSize: 14
                        }

                        Switch {
                            anchors {
                                right: parent.right
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            checked: locationViewshed.frustumOutlineVisible
                            onCheckedChanged: locationViewshed.frustumOutlineVisible = checked;
                        }
                    }

                    ViewshedSlider {
                        titleText: qsTr("Min Distance (m)")
                        parameterValue: locationViewshed.minDistance
                        minValue: 1
                        maxValue: 2000
                        onParameterValueChanged: locationViewshed.minDistance = parameterValue;
                    }

                    ViewshedSlider {
                        titleText: qsTr("Max Distance (m)")
                        parameterValue: locationViewshed.maxDistance
                        minValue: 1
                        maxValue: 2000
                        onParameterValueChanged: locationViewshed.maxDistance = parameterValue;
                    }

                    ViewshedSlider {
                        titleText: qsTr("Vertical Angle")
                        parameterValue: locationViewshed.verticalAngle
                        minValue: 1
                        maxValue: 120
                        onParameterValueChanged: locationViewshed.verticalAngle = parameterValue;
                    }

                    ViewshedSlider {
                        titleText: qsTr("Horizontal Angle")
                        parameterValue: locationViewshed.horizontalAngle
                        minValue: 1
                        maxValue: 120
                        onParameterValueChanged: locationViewshed.horizontalAngle = parameterValue;
                    }

                    ViewshedSlider {
                        titleText: qsTr("Heading")
                        parameterValue: locationViewshed.heading
                        minValue: 1
                        maxValue: 359
                        onParameterValueChanged: locationViewshed.heading = parameterValue;
                    }

                    ViewshedSlider {
                        titleText: qsTr("Pitch")
                        parameterValue: locationViewshed.pitch
                        minValue: 1
                        maxValue: 179
                        onParameterValueChanged: locationViewshed.pitch = parameterValue;
                    }

                    Row {
                        width: parent.width
                        height: 25
                        spacing: 5

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.85
                            text: qsTr("Visible Color")
                            font.pixelSize: 14
                        }

                        Rectangle {
                            id: visibleColorRect
                            anchors {
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            width: 25
                            height: width
                            border {
                                color: "black"
                                width: 1
                            }
                            color: Viewshed.visibleColor()
                            radius: 4

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    visibleColorDialog.open();
                                }
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        height: 25
                        spacing: 5

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width * 0.85
                            text: qsTr("Obstructed Color")
                            font.pixelSize: 14
                        }

                        Rectangle {
                            id: obstructedColorRect
                            anchors {
                                margins: 10
                                verticalCenter: parent.verticalCenter
                            }
                            width: 25
                            height: width
                            border {
                                color: "black"
                                width: 1
                            }
                            color: Viewshed.obstructedColor()
                            radius: 4

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    obstructedColorDialog.open();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ColorDialog {
        id: visibleColorDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        onAccepted: {
            visibleColorRect.color = color;
            Viewshed.setVisibleColor(color);
        }
        onOpened: {
            setColor(visibleColorRect.color);
        }
    }

    ColorDialog {
        id: obstructedColorDialog
        modal: true
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        onAccepted: {
            obstructedColorRect.color = color;
            Viewshed.setObstructedColor(color);
        }
        onOpened: {
            setColor(obstructedColorRect.color);
        }
    }
}
