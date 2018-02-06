// [WriteFile Name=Animate3DSymbols, Category=Scenes]
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
import Esri.ArcGISRuntime 100.3
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data/3D"

    property int missionSize: currentMissionModel.count
    property bool missionReady: missionSize > 0
    property alias following: followButton.checked

    property string headingAtt: "heading";
    property string pitchAtt: "pitch";
    property string rollAtt: "roll";
    property string attrFormat: "[%1]"

    property Graphic routeGraphic

    /**
     * Create SceneView that contains a Scene with the Imagery Basemap
     */

    // Create a scene view
    SceneView {
        id: sceneView
        anchors.fill: parent
        attributionTextVisible: (sceneView.width - mapView.width) > mapView.width // only show attribution text on the widest view

        cameraController: followButton.checked && missionReady ? followController : globeController

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
            id: sceneOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementAbsolute
            }

            SimpleRenderer {
                id: sceneRenderer
                RendererSceneProperties {
                    id: renderProps
                    headingExpression: attrFormat.arg(headingAtt)
                    pitchExpression:  attrFormat.arg(pitchAtt)
                    rollExpression: attrFormat.arg(rollAtt)
                }
            }

            ModelSceneSymbol {
                id: mms
                url: dataPath + "/Bristol/Collada/Bristol.dae"
                scale: 10.0
                heading: 0.0
            }

            Graphic {
                id: graphic3d
                symbol: mms

                geometry: Point {
                    x: 0.
                    y: 0.
                    z: 100.
                    spatialReference: sceneView.spatialReference
                }

                Component.onCompleted: {
                    graphic3d.attributes.insertAttribute(headingAtt, 0.);
                    graphic3d.attributes.insertAttribute(rollAtt, 0.);
                    graphic3d.attributes.insertAttribute(pitchAtt, 0.);
                }
            }
        }
    }

    GlobeCameraController {
        id: globeController
    }

    OrbitGeoElementCameraController {
        id: followController
        targetGeoElement: graphic3d
        cameraDistance: cameraDistanceSlider.value
        cameraPitchOffset: cameraAngle.value
    }

    ListModel {
        id: missionsModel
        ListElement{ name: "Grand Canyon"}
        ListElement{ name: "Hawaii"}
        ListElement{ name: "Pyrenees"}
        ListElement{ name: "Snowdon"}
    }

    ListModel {
        id: currentMissionModel
    }

    GroupBox {
        id: animationGroupBox
        z: 110
        anchors {
            top: sceneView.top
            left: sceneView.left
            margins: 10 * scaleFactor
        }

        Column {
            spacing: 10

            ComboBox {
                id: missionList
                enabled: !playButton.checked
                model: missionsModel
                textRole: "name"
                width: 150 * scaleFactor
                onCurrentTextChanged: {
                    changeMission(currentText);
                    progressSlider.value = 0;
                }
            }

            Button {
                id: playButton
                checked: false
                checkable: true
                enabled: missionReady
                text: checked ? "pause" : "play"
            }

            Text {
                id: progressTitle
                text: "progress"
                color: "white"
            }

            Slider {
                id: progressSlider
                minimumValue: 0
                maximumValue: missionSize
                enabled : missionReady
                width: Math.max(implicitWidth, 150) * scaleFactor
            }

            CheckBox {
                id: followButton
                enabled: missionReady
                text: "follow"
                checked: true
            }
        }
    }

    GroupBox {
        id: cameraGroupBox
        z: 110
        anchors {
            top: sceneView.top
            right: sceneView.right
            margins: 10 * scaleFactor
        }

        Column {
            spacing: 10

            Text {
                id: distTitle
                text: "zoom"
                enabled: following && missionReady
                color: "white"
            }

            Slider {
                id: cameraDistanceSlider
                enabled: following && missionReady
                minimumValue: followController.minCameraDistance
                maximumValue: 5000.0
                value: 500.0
                width: Math.max(implicitWidth, 100) * scaleFactor
            }

            Text {
                id: angleTitle
                text: "angle"
                enabled: following && missionReady
                color: "white"
            }

            Slider {
                id: cameraAngle
                enabled: following && missionReady
                minimumValue: followController.minCameraPitchOffset
                maximumValue: followController.maxCameraPitchOffset
                value: 45.0
                width: Math.max(implicitWidth, 100) * scaleFactor
            }

            Text {
                id: speedTitle
                text: "speed"
                enabled: missionReady
                color: "white"
            }

            Slider {
                id: animationSpeed
                enabled: missionReady
                minimumValue: 1
                maximumValue: 100
                value: 50
                width: Math.max(implicitWidth, 100) * scaleFactor
            }
        }
    }

    Rectangle {
        id: mapFrame
        anchors {left:sceneView.left; bottom: sceneView.bottom}
        anchors.margins: 10 * scaleFactor
        anchors.bottomMargin: 25 * scaleFactor
        width: Math.max(sceneView.width * .2, 128 * scaleFactor)
        height: Math.max(sceneView.height * .4, 128 * scaleFactor)
        color: "black"
        z: 100
        clip: true

        GroupBox {
            id: mapZoomBox
            z: 120
            anchors {
                top: mapFrame.top
                margins: 10 * scaleFactor
            }
            width: mapFrame.width

            Row {
                spacing: 10

                Button {
                    id: mapZoomIn
                    anchors.margins: 10
                    text: "+"
                    width: height
                    onClicked: zoomMapIn()
                }

                Button {
                    id: mapZoomOut
                    anchors.margins: 10
                    text: "-"
                    width: height
                    onClicked: zoomMapOut()
                }
            }
        }

        MapView {
            id: mapView
            attributionTextVisible: !sceneView.attributionTextVisible
            anchors {
                fill: mapFrame
                margins: 2 * scaleFactor
            }

            Map {
                BasemapImagery {}
            }

            GraphicsOverlay {
                id: graphicsOverlay

                Graphic {
                    id: graphic2d
                    symbol: plane2DSymbol
                }
            }

            MouseArea {
                anchors.fill: parent
                onPressed: mouse.accepted;
                onWheel: wheel.accepted;
            }
        }
    }

    SimpleLineSymbol {
        id: routeSymbol
        style: Enums.SimpleLineSymbolStyleSolid
        color: Qt.rgba(1.0, 0.0, 0.0, 1)
        width: 1
        antiAlias: true
    }

    SimpleMarkerSymbol {
        id: plane2DSymbol
        style: Enums.SimpleMarkerSymbolStyleTriangle
        color: "blue"
        size: 10
    }

    Timer {
        id: timer
        interval: Math.max(animationSpeed.maximumValue - animationSpeed.value,1);
        running: playButton.checked;
        repeat: true
        onTriggered: animate();
    }

    FileFolder {
        id: missionsFolder
        path: System.resolvedPath(dataPath) + "/Missions/"
    }

    function changeMission(missionName) {
        currentMissionModel.clear();
        progressSlider.value = 0;
        if (!missionsFolder.exists)
            return;

        var fileName = missionName.replace(/\s/g, '') + ".csv";
        var fileContents = missionsFolder.readTextFile(fileName);
        var lines = fileContents.split("\n");
        for (var i = 0; i < lines.length; i++) {
            var dataParts = lines[i].split(",");
            if (dataParts.length !== 6)
                continue;

            currentMissionModel.append({
                "lon":dataParts[0],
                "lat":dataParts[1],
                "elevation":dataParts[2],
                "heading":dataParts[3],
                "pitch":dataParts[4],
                "roll":dataParts[5],
            })
        }

        if (missionSize === 0)
            return;

        // create polyline builder and fill with points
        // for the mission polyline
        var rtBldr = ArcGISRuntimeEnvironment.createObject(
            "PolylineBuilder", {spatialReference: SpatialReference.createWgs84()});
        for (var j = 0; j < currentMissionModel.count; j++) {
            var missionData = currentMissionModel.get(j);
            rtBldr.addPointXY(missionData.lon, missionData.lat);
        }

        var firstData = currentMissionModel.get(0);
        var firstPos = createPoint(firstData);

        // update model graphic's attributes
        graphic3d.attributes.replaceAttribute(headingAtt, firstData.heading);
        graphic3d.attributes.replaceAttribute(rollAtt, firstData.roll);
        graphic3d.attributes.replaceAttribute(pitchAtt, firstData.pitch);

        // update model graphic's geomtry
        graphic3d.geometry = firstPos;

        // update the 2d graphic
        graphic2d.geometry = firstPos;
        plane2DSymbol.angle = firstData.heading;

        if (!routeGraphic) {
            // create route graphic with the route symbol
            routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic");
            routeGraphic.symbol = routeSymbol;

            // add route graphic to the graphics overlay
            graphicsOverlay.graphics.insert(0, routeGraphic);
        }

        // update route graphic's geomtry
        routeGraphic.geometry = rtBldr.geometry;

        mapView.setViewpointGeometryAndPadding(routeGraphic.geometry, 30);
    }

    function animate() {
        if (progressSlider.value < missionSize ) {
            var missionData = currentMissionModel.get(progressSlider.value);
            var newPos = createPoint(missionData);

            graphic3d.geometry = newPos;
            graphic3d.attributes.replaceAttribute(headingAtt, missionData.heading);
            graphic3d.attributes.replaceAttribute(pitchAtt, missionData.pitch);
            graphic3d.attributes.replaceAttribute(rollAtt, missionData.roll);

            // update the 2d graphic
            graphic2d.geometry = newPos;
            plane2DSymbol.angle = missionData.heading;
        }

        nextFrameRequested();
    }

    function zoomMapIn(){
        mapView.setViewpointScale(mapView.mapScale / 5.0);
    }

    function zoomMapOut(){
        mapView.setViewpointScale(mapView.mapScale * 5.0);
    }

    function nextFrameRequested() {
        progressSlider.value = progressSlider.value + 1;
        if (progressSlider.value >= missionSize)
           progressSlider.value = 0;
    }

    function createPoint(missionData) {
        return ArcGISRuntimeEnvironment.createObject(
            "Point", {
                x: missionData.lon,
                y: missionData.lat,
                z: missionData.elevation,
                spatialReference: SpatialReference.createWgs84()
            });
    }
}
