// [WriteFile Name=GetElevationAtPoint, Category=Scenes]
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

import Esri.ArcGISRuntime 100.9
import QtQuick 2.6
import QtQuick.Controls 2.2

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // The position on the surface that was last queried for altitude, so the marker can be set in the onLocationToElevationResultChanged callback.
    property Point lastQueriedSurfacePos : null

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            id: scene
            BasemapImagery {}

            Surface {
                id: elevationSurface
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }

                // When location has changed, set the elevation text to display new elevation, and move the elevationMarker to the queried-for position on the surface.
                onLocationToElevationResultChanged:
                    if(locationToElevationStatus === Enums.TaskStatusCompleted) {
                        // Display elevation value in meters, round to a single decimal place.
                        elevationDisplaytext.text = "Elevation : " + Math.round(locationToElevationResult * 10) / 10 + "m"
                        elevationMarker.geometry = lastQueriedSurfacePos;
                    }
            }
        }

        // A graphics overlay to display the elevaton marker.
        GraphicsOverlay {
            id: graphicsOverlay
            Graphic {
                id: elevationMarker
                symbol: elevationMarkerSymbol
            }
        }

        // A simple red dot, displays the point elevation is queried at.
        SimpleMarkerSymbol {
            id: elevationMarkerSymbol
            style: Enums.SimpleMarkerSymbolStyleCircle
            color: "red"
            size: 10
        }

        // When the mouse is clicked, convert mouse screen-position to position on surface, then invoke a query for the elevation of that position.
        onMouseClicked: {
            lastQueriedSurfacePos = sceneView.screenToBaseSurface(mouse.x, mouse.y)
            elevationSurface.locationToElevation(lastQueriedSurfacePos)
        }

        // Once the scene view has loaded, apply the camera.
        Component.onCompleted: {
            setViewpointCameraAndWait(camera);
        }
    }

    // Create the camera to be used as the scene view's viewpoint, looking at the Himalayan mountain range.
    Camera {
        id: camera
        location: Point {
            x: 83.9 // Longitude
            y: 28.4 // Latitude
            z: 10010.0 // Altiude
            spatialReference: SpatialReference { wkid: 4326 }
        }
        heading: 10.0
        pitch: 83.9
        roll: 0.0
    }

    // Background rectangle for elevation display
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 30
        }

        color: Qt.rgba(0.2, 0.2, 0.2, 0.65);
        height: childrenRect.height

        // Elevation display text
        Text {
            id: elevationDisplaytext
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            color: "white"
            padding: 15
            font.pointSize: 32

            // For vertical screens, keep the text within the bounding box via scaling down.
            scale: Math.min(1, (parent.width - padding) / contentWidth)

            // Display elevation value in meters, round to a single decimal place.
            text: "Elevation : 0"
        }
    }

    // Display an indictor when the elevation query is running, since it might take a couple of seconds
    BusyIndicator {
        running: elevationSurface.locationToElevationStatus === Enums.TaskStatusInProgress
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) / 6.0
        height: width
    }
}
