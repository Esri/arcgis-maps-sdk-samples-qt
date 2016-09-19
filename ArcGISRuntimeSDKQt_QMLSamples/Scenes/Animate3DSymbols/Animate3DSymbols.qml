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
import QtQuick.Extras 1.4
import Esri.ArcGISRuntime 100.0
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

    property string headingAtt: "HEADING";
    property string pitchAtt: "PITCH";
    property string rollAtt: "ROLL";
    property string angleAtt: "ANGLE";

    property var graphic3d;

    /**
     * Create SceneView that contains a Scene with the Imagery Basemap
     */

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
            id: sceneOverlay

            LayerSceneProperties {
                surfacePlacement: Enums.SurfacePlacementAbsolute
            }

            SimpleRenderer {
                id: sceneRenderer
                RendererSceneProperties {
                    id: renderProps
                    headingExpression: headingAtt
                    pitchExpression: pitchAtt
                    rollExpression: rollAtt
                }
            }

            ModelSceneSymbol {
                id: mms
                url: System.resolvedPath(dataPath) + "/SkyCrane/SkyCrane.lwo"
                scale: 0.01
                heading: 180
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = followButton.checked
            onWheel: wheel.accepted = followButton.checked
        }
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
        anchors {top: sceneView.top; left: sceneView.left; margins: 10 * scaleFactor}

        Column {
            spacing: 10

            ComboBox {
                id: missionList
                model: missionsModel
                textRole: "name"
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
                renderType: Text.NativeRendering
            }

            Slider {
                id: progressSlider
                minimumValue: 0
                maximumValue: missionSize
                enabled : missionReady
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
        anchors {top: sceneView.top; right: sceneView.right; margins: 10 * scaleFactor}

        Column {
            spacing: 10

            Text {
                id: distTitle
                text: "zoom"
                enabled: following && missionReady
                renderType: Text.NativeRendering
            }

            Slider {
                id: cameraDistance
                enabled: following && missionReady
                minimumValue: 10.0
                maximumValue: 500.0
                value: 200.0
            }

            Text {
                id: angleTitle
                text: "angle"
                enabled: following && missionReady
                renderType: Text.NativeRendering
            }

            Slider {
                id: cameraAngle
                enabled: following && missionReady
                minimumValue: 0.0
                maximumValue: 180.0
                value: 75.0
            }

            Text {
                id: speedTitle
                text: "speed"
                enabled: missionReady
                renderType: Text.NativeRendering
            }

            Slider {
                id: animationSpeed
                enabled: missionReady
                minimumValue: 50
                maximumValue: 200
                value: 50
            }
        }
    }

    Rectangle {
        id: mapFrame
        anchors {left:sceneView.left; bottom: sceneView.bottom}
        width: Math.max(sceneView.width * .2, 128 * scaleFactor)
        height: Math.max(sceneView.height * .4, 128 * scaleFactor)
        color: "black"
        z: 100
        clip: true

        GroupBox {
            id: mapZoomBox
            z: 120
            anchors {top: mapFrame.top; margins: 10 * scaleFactor}
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
            anchors {fill: mapFrame; margins: 2 * scaleFactor}

            Map {
                BasemapImagery {}
            }

            GraphicsOverlay {
                id: graphicsOverlay
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
        style: Enums.SimpleFillSymbolStyleSolid
        color: Qt.rgba(1.0, 0.0, 0.0, 1)
        width: 1
        antiAlias: true
    }

    Timer {
        id: timer
        interval: 210 - animationSpeed.value;
        running: playButton.checked;
        repeat: true
        onTriggered: animate();
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

    FileFolder{
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

        sceneOverlay.graphics.clear();
        graphic3d = ArcGISRuntimeEnvironment.createObject("Graphic");
        graphic3d.symbol = mms;

        sceneOverlay.graphics.append(graphic3d);

        var rtBldr = ArcGISRuntimeEnvironment.createObject(
            "PolylineBuilder", {spatialReference: SpatialReference.createWgs84()});
        for (var j = 0; j < currentMissionModel.count; j++) {
            var missionData = currentMissionModel.get(j);
            rtBldr.addPointXY(missionData.lon, missionData.lat);
        }

        var routeGraphic = ArcGISRuntimeEnvironment.createObject("Graphic");
        routeGraphic.geometry = rtBldr.geometry;
        routeGraphic.symbol = routeSymbol;
        graphicsOverlay.graphics.append(routeGraphic);

        var firstData = currentMissionModel.get(0);
        var firstPos = createPoint(firstData);
        graphic3d.geometry = firstPos;
        graphic3d.attributes.insertAttribute(headingAtt, firstData.heading);
        graphic3d.attributes.insertAttribute(rollAtt, firstData.roll);
        graphic3d.attributes.insertAttribute(pitchAtt, firstData.pitch);
        setCamera(firstPos, firstData.heading);
        mapView.setViewpointGeometryAndPadding(routeGraphic.geometry, 200);
    }

    function animate() {
        if (progressSlider.value < missionSize ) {
            var missionData = currentMissionModel.get(progressSlider.value);
            var newPos = createPoint(missionData);

            graphic3d.geometry = newPos;
            graphic3d.attributes.replaceAttribute(headingAtt, missionData.heading);
            graphic3d.attributes.replaceAttribute(pitchAtt, missionData.pitch);
            graphic3d.attributes.replaceAttribute(rollAtt, missionData.roll);

            if (followButton.checked)
                setCamera(newPos, missionData.heading);
            else
                sceneView.update();
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

    function setCamera(point, headingVal) {
        var cam = ArcGISRuntimeEnvironment.createObject(
            "Camera", {
                location: point,
                distance: cameraDistance.maximumValue - cameraDistance.value,
                heading: headingVal,
                pitch: cameraAngle.value,
                roll: 0.0
            });

        sceneView.setViewpointCamera(cam);
    }
}
