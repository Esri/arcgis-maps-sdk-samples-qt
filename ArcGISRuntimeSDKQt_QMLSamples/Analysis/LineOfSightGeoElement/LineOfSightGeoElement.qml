// [WriteFile Name=LineOfSightGeoElement, Category=Analysis]
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
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1
import Qt.labs.platform 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/3D/dolmus_3ds/dolmus.3ds";
    property int waypointIndex: 0

    // Mutable point of where observer is going to be.
    PointBuilder {
        id: observationPointBuilder
        x: -73.9853
        y: 40.7484
        z: 200
        SpatialReference { wkid: 4326 }
    }

    // List of all waypoints taxi travels around.
    property list<Point> waypoints: [
        Point {
            x: -73.984513
            y: 40.748469
            z: 2
            spatialReference: SpatialReference { wkid: 4326 }
        },
        Point {
            x: -73.985068
            y: 40.747786
            z: 2
            spatialReference: SpatialReference { wkid: 4326 }

        },
        Point {
            x: -73.983452
            y: 40.747091
            z: 2
            spatialReference: SpatialReference { wkid: 4326 }
        },
        Point {
            x: -73.982961
            y: 40.747762
            z: 2
            spatialReference: SpatialReference { wkid: 4326 }
        }
    ]

    Camera {
        id: camera
        location: observationPointBuilder.geometry
        distance: 700
        heading: -30
        pitch: 45
        roll: 0
    }

    SceneView {
        id: view
        anchors.fill: parent

        // Slider that alters the z-height of the observation point.

        Rectangle {
            anchors {
                margins: 5
                left: parent.left
                top: parent.top
                bottom: view.attributionTop
            }
            color: Qt.rgba(0.7, 0.7, 0.7, 0.7);
            radius: 10
            width: childrenRect.width

            Slider {
                id: heightSlider

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    margins: 5
                }
                from: 150
                to: 300
                stepSize: 10
                value: observationPointBuilder.z
                orientation: Qt.Vertical
                onMoved: {
                    observationPointBuilder.z = value;
                    observer.geometry = observationPointBuilder.geometry;
                }
            }
        }

        Scene {
            BasemapImagery { }

            Surface {
                ArcGISTiledElevationSource {
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }

            ArcGISSceneLayer {
                url: "https://tiles.arcgis.com/tiles/z2tnIkrLQ2BRzr6P/arcgis/rest/services/New_York_LoD2_3D_Buildings/SceneServer/layers/0"
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    view.setViewpointCameraAndSeconds(camera, 0);
                }
            }
        }

        // Line of sight between observer and taxi.
        AnalysisOverlay {
            GeoElementLineOfSight {
                id: lineOfSight
                observerGeoElement: observer
                targetGeoElement: taxi
            }
        }

        // Taxi and observer added to overlay.
        GraphicsOverlay {
            LayerSceneProperties {
                // Note all Z-heights are relative to surface.
                surfacePlacement: Enums.SurfacePlacementRelative
            }

            SimpleRenderer {
                // Specific attribute so taxi will orient itself correctly.
                RendererSceneProperties {
                    headingExpression: "[HEADING]"
                }
            }

            // Draw observer.
            Graphic {
                id: observer
                geometry: observationPointBuilder.geometry
                SimpleMarkerSymbol {
                    style: Enums.SimpleMarkerSymbolStyleCircle
                    color: "red"
                    size: 5
                }
            }

            // Draw taxi
            Graphic {
                id: taxi
                geometry: waypoints[0]
                selected: lineOfSight.targetVisibility === Enums.LineOfSightTargetVisibilityVisible
                Component.onCompleted: {
                    // Append additional attribute for orientation.
                    attributes.insertAttribute("HEADING", 0.0);
                }
                ModelSceneSymbol {
                    id: taxiSymbol
                    url: dataPath
                    scale: 1.0
                }
            }
        }
    }

    Timer {
        id: animation
        interval: 100
        running: true
        repeat: true
        onTriggered: animate();
    }

    LinearUnit {
        id: metres
        linearUnitId: Enums.LinearUnitIdMeters
    }

    AngularUnit {
        id: degrees
         angularUnitId: Enums.AngularUnitIdDegrees
    }

    function animate() {
        // Get current goal waypoint and taxi location.
        var waypoint = waypoints[waypointIndex];
        var location = taxi.geometry;

        // Discover the azimuth between these two points.
        var distance = GeometryEngine.distanceGeodetic(
                    location, waypoint, metres, degrees,
                    Enums.GeodeticCurveTypeGeodesic);

        // Move the taxi location along 1 metre in the direction of the azimuth. Update the taxi.
        var points = GeometryEngine.moveGeodetic(
                    [location], 1.0, metres, distance.azimuth1, degrees,
                    Enums.GeodeticCurveTypeGeodesic);

        if (points.length > 0) {
            location = points[0];
        }

        taxi.geometry = location;
        taxi.attributes.replaceAttribute("HEADING", distance.azimuth1);

        // When taxi is close enough to goal point then update waypoint.
        if (distance.distance <= 2) {
            waypointIndex = (waypointIndex + 1) % waypoints.length;
        }
    }
}
