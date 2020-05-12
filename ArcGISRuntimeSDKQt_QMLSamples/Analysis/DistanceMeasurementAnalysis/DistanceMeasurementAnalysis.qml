// [WriteFile Name=DistanceMeasurementAnalysis, Category=Analysis]
// [Legal]
// Copyright 2018 Esri.

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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600
    
    property bool isNavigating: false

    SceneView {
        id: sceneView
        anchors.fill: parent

        property bool isPressAndHeld: false

        // Declare a Scene
        Scene {
            id: scene
            // Set the basemap
            BasemapImagery {}

            // Add a Scene Layer
            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_Brest/SceneServer/layers/0"
            }

            // Set the Surface
            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
                ArcGISTiledElevationSource {
                    url: "https://tiles.arcgis.com/tiles/d3voDfTFbHOCRwVR/arcgis/rest/services/MNT_IDF/ImageServer"
                }
            }

            // set initial viewpoint
            ViewpointCenter {
                center: locationDistanceMeasurement.startLocation
                targetScale: 200

                Camera {
                    location: locationDistanceMeasurement.startLocation
                    distance: 400
                    pitch: 45
                    heading: 0
                    roll: 0
                }
            }
        }        

        // Declare an AnalysisOverlay
        AnalysisOverlay {
            id: analysisOverlay


            // Declare a Location Distance Measurement
            LocationDistanceMeasurement {
                id: locationDistanceMeasurement

                property string unitLabel: unitSystem === Enums.UnitSystemMetric ? "m" : "ft"

                // set unit system
                unitSystem: Enums.UnitSystemMetric
                // set the start point
                startLocation: Point {
                    x: -4.494677
                    y: 48.384472
                    z: 24.772694
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                // set the end point
                endLocation: Point {
                    x: -4.495646
                    y: 48.384377
                    z: 58.501115
                    spatialReference: SpatialReference { wkid: 4326 }
                }
                // connect to distance change signals
                onDirectDistanceChanged: directDistanceText.text = directDistance.value.toFixed(2) + " %1".arg(unitSystem === Enums.UnitSystemMetric ? "m" : "ft")
                onHorizontalDistanceChanged: horizontalDistanceText.text = horizontalDistance.value.toFixed(2) + " %1".arg(unitSystem === Enums.UnitSystemMetric ? "m" : "ft")
                onVerticalDistanceChanged: verticalDistanceText.text = verticalDistance.value.toFixed(2) + " %1".arg(unitSystem === Enums.UnitSystemMetric ? "m" : "ft")
            }
        }

        // handle mouse signals to update the analysis

        // When the mouse is pressed and held, start updating the distance analysis end point
        onMousePressedAndHeld: {
            isPressAndHeld = true;
            sceneView.screenToLocation(mouse.x, mouse.y);
        }

        // When the mouse is released...
        onMouseReleased: {
            // Check if the mouse was released from a pan gesture
            if (isNavigating) {
                isNavigating = false;
                return;
            }

            // Ignore if Right click
            if (mouse.button === Qt.RightButton)
                return;

            // If pressing and holding, do nothing
            if (isPressAndHeld)
                isPressAndHeld = false;
            // Else get the location from the screen coordinates
            else
                sceneView.screenToLocation(mouse.x, mouse.y);
        }

        // Set a flag when mousePressed signal emits
        onMousePressed: {
            isNavigating = false;
        }

        // Update the distance analysis when the mouse moves if it is a press and hold movement
        onMousePositionChanged: {
            if (isPressAndHeld)
                sceneView.screenToLocation(mouse.x, mouse.y);
        }

        // When screenToLocation completes...
        onScreenToLocationCompleted: {
            if (isPressAndHeld)
                locationDistanceMeasurement.endLocation = location;
            else
                locationDistanceMeasurement.startLocation = location;
        }

        // Set a flag when viewpointChanged signal emits
        onViewpointChanged: {
            isNavigating = true;
        }
    }

    Rectangle {
        anchors {
            fill: resultsColumn
            margins: -5
        }
        color: "black"
        opacity: 0.5
        radius: 5
    }

    Column {
        id: resultsColumn
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        spacing: 5

        Row {
            spacing: 5
            Text {
                text: "Direct Distance:"
                color: "white"
            }
            Text {
                id: directDistanceText
                color: "white"
            }
        }
        Row {
            spacing: 5
            Text {                
                text: "Vertical Distance:"
                color: "white"
            }
            Text {
                id: verticalDistanceText
                color: "white"
            }
        }
        Row {
            spacing: 5
            Text {
                text: "Horizontal Distance:"
                color: "white"
            }
            Text {
                id: horizontalDistanceText
                color: "white"
            }
        }
        Row {
            spacing: 5
            Text {
                text: "Unit System:"
                color: "white"
            }
            ComboBox {
                id: comboBox
                property int modelWidth: 0
                width: modelWidth + leftPadding + rightPadding + indicator.width
                model: ["Metric", "Imperial"]
                onCurrentTextChanged: {
                    if (currentText === "Metric")
                        locationDistanceMeasurement.unitSystem = Enums.UnitSystemMetric;
                    else
                        locationDistanceMeasurement.unitSystem = Enums.UnitSystemImperial;
                }
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }

                TextMetrics {
                    id: metrics
                    font: comboBox.font
                }
            }
        }
    }
}
