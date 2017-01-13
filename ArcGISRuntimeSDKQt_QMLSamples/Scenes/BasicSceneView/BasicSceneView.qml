// [WriteFile Name=BasicSceneView, Category=Scenes]
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600
    property real scaleFactor: 1.0
    property url url_SharingImageryWithLabelsMap: "http://www.arcgis.com/sharing/rest/content/items/716b600dbbac433faa4bec9220c76b3a";

    //! [create the scene with a basemap and surface]
    // Create a scene view
    SceneView {
        id: sceneView
        anchors.fill: parent

        // create a scene...scene is a default property of sceneview
        // and thus will get added to the sceneview
        Scene {
            // add a basemap
            BasemapImagery {}

            // add a surface...surface is a default property of scene
            Surface {
                // add an arcgis tiled elevation source...elevation source is a default property of surface
                ArcGISTiledElevationSource {
                    url: "http://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCameraAndWait(camera);
        }
    }
    //! [create the scene with a basemap and surface]

    //! [create the camera to be used as the scene view's viewpoint]
    Camera {
        id: camera
        heading: 10.0
        pitch: 80.0
        roll: 300.0

        Point {
            x: 83.9
            y: 28.4
            z: 10010.0
            spatialReference: SpatialReference.createWgs84()
        }
    }
    //! [create the camera to be used as the scene view's viewpoint]


    Rectangle {
        width: 275 * scaleFactor
        height: 200 * scaleFactor
        radius: 4
        color: "lightgray"
        clip: true
        border {
            color: "black"
            width: 2
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        ScrollView {
            anchors {
                margins: 10
                fill: parent
            }

            Column {
                spacing: 10

                Button {
                    width: 200 * scaleFactor
                    height: 40 * scaleFactor
                    text: "Portal item basemap"
                    onClicked: {
                        var newBasemap = ArcGISRuntimeEnvironment.createObject("Basemap");
                        var newPortalItem = ArcGISRuntimeEnvironment.createObject("PortalItem");
                        newPortalItem.url = url_SharingImageryWithLabelsMap;
                        newBasemap.item = newPortalItem;
                        sceneView.scene.basemap = newBasemap;
                    }
                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "AGS basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
//                        var tiledlayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        tiledlayer.url = url_UsaCachedMap;
//                        basemap.baseLayers.append(tiledlayer);
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "AGOL basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
//                        var tiledlayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        tiledlayer.url = url_UsaThematicMapServer
//                        basemap.baseLayers.append(tiledlayer);
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Imagery basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapImagery");
//                        setBasemap(basemap);
//                    }
//                }

                Button {
                    width: 200 * scaleFactor
                    height: 40 * scaleFactor
                    text: "Imagery w/ labels\nbasemap"
                    onClicked: {
                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapImageryWithLabels");
                        sceneView.scene.basemap = basemap;
                    }
                }

                Button {
                    width: 200 * scaleFactor
                    height: 40 * scaleFactor
                    text: "Light Gray basemap"
                    onClicked: {
                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapLightGrayCanvas");
                        sceneView.scene.basemap = basemap;
                    }
                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Nat Geo basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapNationalGeographic");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Oceans basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapOceans");
//                        setBasemap(basemap);
//                    }
//                }

                //            Button {
                //                width: 200 * scaleFactor
                //                height: 40 * scaleFactor
                //                text: "OSM basemap"
                //                onClicked: {
                //                    var basemap = ArcGISRuntimeEnvironment.createObject("BasemapOpenStreetMap");
                //                    setBasemap(basemap);
                //                }
                //            }

                Button {
                    width: 200 * scaleFactor
                    height: 40 * scaleFactor
                    text: "Streets basemap"
                    onClicked: {
                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreets");
                        sceneView.scene.basemap = basemap;
                    }
                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Terrain basemap"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapTerrainWithLabels");
//                        setBasemap(basemap);
//                    }
//                }

                Button {
                    width: 200 * scaleFactor
                    height: 40 * scaleFactor
                    text: "Topo basemap"
                    onClicked: {
                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapTopographic");
                        sceneView.scene.basemap = basemap;
                       }
                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Imagery w/ labels vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapImageryWithLabelsVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Streets vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreetsVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Topo vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapTopographicVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Terrain w/ labels vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapTerrainWithLabelsVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Light Gray Canvas vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapLightGrayCanvasVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Navigation vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapNavigationVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Streets Night vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreetsNightVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Streets w/ Relief vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreetsWithReliefVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Dark Gray Canvas vector"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("BasemapDarkGrayCanvasVector");
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Basemap from sharing Url"
//                    onClicked: {
//                        setBasemap(ArcGISRuntimeEnvironment.createObject("Basemap", {url : url_imageryWithLabelsData}));
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Basemap from details Url"
//                    onClicked: {
//                        setBasemap(ArcGISRuntimeEnvironment.createObject("Basemap", {url : url_imageryWithLabelsDetails}));
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Basemap from viewer Url"
//                    onClicked: {
//                        setBasemap(ArcGISRuntimeEnvironment.createObject("Basemap", {url : url_imageryWithLabelsViewer}));
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Set base and\nref layers"
//                    onClicked: {
//                        var basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
//                        var tiledlayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        tiledlayer.url = url_WorldDarkGrayBaseMapServer;
//                        var reflayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        reflayer.url = url_WorldDarkGrayCanvasReference;
//                        basemap.baseLayers.append(tiledlayer);
//                        basemap.referenceLayers.append(reflayer);
//                        setBasemap(basemap);
//                    }
//                }

//                Button {
//                    width: 200 * scaleFactor
//                    height: 40 * scaleFactor
//                    text: "Append base and\nref layers"
//                    onClicked: {
//                        var tiledlayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        tiledlayer.url = url_WorldShadedReliefMapServer;
//                        var reflayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer");
//                        reflayer.url = url_WorldDarkGrayCanvasReference;
//                        curBasemap.referenceLayers.append(reflayer);
//                        curBasemap.baseLayers.remove(0);
//                        curBasemap.baseLayers.append(tiledlayer);
//                    }
//                }
            }
        }
    }

    // Neatline rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
