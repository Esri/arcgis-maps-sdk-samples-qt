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

    property real size: 0.01
    property int maxZ: 1000
    property var colors: []
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

        GraphicsOverlay {
            id: graphicsOverlay

            SimpleRenderer {
                RendererSceneProperties {
                    extrusionMode: Enums.ExtrusionModeBaseHeight
                    extrusionExpression: "height"
                }
            }
        }

        Component.onCompleted: {
            // set viewpoint to the specified camera
            setViewpointCamera(camera);
            createGraphics();
        }
    }

    // create the camera to be used as the scene view's viewpoint
    Camera {
        id: camera
        location: Point {
            x: 83.9
            y: 28.4
            z: 10010.0
            spatialReference: SpatialReference.createWgs84()
        }
        heading: 10.0
        pitch: 80.0
        roll: 300.0
    }

    function createGraphics(){
        var lat = camera.location.x;
        var lng = camera.location.y + 0.2;

        // add a set of colors to pick from
        addColors();

        // create a random set of points
        var points = [];
        for (var i = 0; i <= 100; i++) {
            var point = ArcGISRuntimeEnvironment.createObject("Point", {x:i / 10 * (size * 2) + lat, y:i % 10 * (size * 2) + lng, spatialReference:sceneView.spatialReference});
            points.push(point);
        }

        // for each point construct a polygon by manipulating the co-ordinates
        points.forEach(function(item){
            var randNum = Math.floor(Math.random() * 6);
            var z = maxZ * randNum;
            var newPoints = [createPoint(item.x, item.y, z),
                             createPoint(item.x + size, item.y, z),
                             createPoint(item.x + size, item.y + size, z),
                             createPoint(item.x, item.y + size, z)];

            // create a simple fill symbol
            var sfs = ArcGISRuntimeEnvironment.createObject("SimpleFillSymbol");
            sfs.style = Enums.SimpleFillSymbolStyleSolid;
            // assign a random color from the list
            sfs.color = colors[randNum];

            // create a graphic using the fill symbol
            var graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {symbol: sfs, geometry: createPolygonFromPoints(newPoints)});
            graphic.attributes.insertAttribute("height", z);
            graphicsOverlay.graphics.append(graphic);
        });
    }

    // create a polygon from a list of points
    function createPolygonFromPoints(pointsList) {
        var polygonBuilder = ArcGISRuntimeEnvironment.createObject("PolygonBuilder");
        polygonBuilder.spatialReference = sceneView.spatialReference;

        pointsList.forEach(function(pnt){
            polygonBuilder.addPoint(pnt);
        });
        return polygonBuilder.geometry;
    }

    // create a point
    function createPoint(x, y, z) {
        return ArcGISRuntimeEnvironment.createObject("Point", {x:x, y:y, z:z, spatialReference: sceneView.spatialReference});
    }

    // add colors to array
    function addColors() {
        colors.push("white");
        colors.push("black");
        colors.push("turquoise");
        colors.push("green");
        colors.push("red");
        colors.push("blue");
        colors.push("purple");
    }
}
