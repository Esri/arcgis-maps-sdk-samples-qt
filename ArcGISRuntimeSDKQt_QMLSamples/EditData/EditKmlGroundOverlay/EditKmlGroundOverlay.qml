// [WriteFile Name=EditKmlGroundOverlay, Category=EditData]
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
import QtQuick.Controls 2.3
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/raster/"

    SceneView {
        id: sceneView
        anchors.fill: parent

        Scene {
            BasemapImagery {}

            // Create a KML Layer
            KmlLayer {
                id: kmlLayer
                // Create a KML Dataset
                KmlDataset {
                    // Create a Ground Overlay by assigning an icon and geometry
                    KmlGroundOverlay {
                        id: groundOverlay
                        rotation: -3.046024799346924
                        KmlIcon {
                            url: dataPath + "1944.jpg"
                        }
                        Envelope {
                            id: env
                            xMin: -123.066227926904
                            yMin: 44.04736963555683
                            xMax: -123.0796942287304
                            yMax: 44.03878298600624
                            SpatialReference {
                                wkid: 4326
                            }
                        }
                    }
                }

                // set viewpoint to the ground overlay
                onLoadStatusChanged: {
                    if (loadStatus !== Enums.LoadStatusLoaded)
                        return;

                    const camera = ArcGISRuntimeEnvironment.createObject("Camera", {
                                                                             location: env.center,
                                                                             distance: 1250,
                                                                             heading: 45,
                                                                             pitch: 60,
                                                                             roll: 0
                                                                         });

                    sceneView.setViewpointCamera(camera);
                }
            }
        }
    }

    Rectangle {
        anchors.fill: slider
        radius: 5
    }

    Slider {
        id: slider
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        from: 0
        to: 1
        value: 1
        stepSize: 0.1
        onValueChanged: {
            // modify the overlay's color/alpha value
            groundOverlay.color = Qt.rgba(0, 0, 0, value);
        }
    }
}
