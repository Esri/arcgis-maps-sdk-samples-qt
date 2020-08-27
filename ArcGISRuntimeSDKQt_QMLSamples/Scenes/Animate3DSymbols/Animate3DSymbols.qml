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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath +  "/ArcGIS/Runtime/Data/3D"
    readonly property string headingAtt: "heading";
    readonly property string pitchAtt: "pitch";
    readonly property string rollAtt: "roll";
    readonly property string attrFormat: "[%1]"
    property int missionSize: currentMissionModel.count
    property bool missionReady: missionSize > 0
    property alias following: followButton.checked

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
                    url: "https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"
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

        GridLayout {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: sceneView.attributionTop
                margins: 10
            }

            columns: 2

            ComboBox {
                id: missionList
                property real modelWidth: 0
                Layout.minimumWidth: leftPadding + rightPadding + indicator.width + modelWidth
                enabled: !playButton.checked
                model: missionsModel
                textRole: "name"

                onModelChanged: {
                    for (let i = 0; i < missionsModel.count; ++i) {
                        textMetrics.text = missionsModel.get(i).name;
                        modelWidth = Math.max(modelWidth, textMetrics.width);
                    }
                }

                onCurrentTextChanged: {
                    changeMission(currentText);
                    progressSlider.value = 0;
                }

                TextMetrics {
                    id: textMetrics
                    font: missionList.font
                }

                Component.onCompleted: missionList.currentTextChanged()
            }

            LabeledSlider {
                id: cameraDistance
                Layout.alignment: Qt.AlignRight
                from: followController.minCameraDistance
                to: 5000.0
                value: 500.0
                text: "zoom"
            }

            RowLayout {
                Button {
                    id: playButton
                    checked: false
                    checkable: true
                    enabled: missionReady
                    text: checked ? "pause" : "play"
                }

                Button {
                    id: followButton
                    Layout.alignment: Qt.AlignRight
                    enabled: missionReady
                    text: checked? "fixed" : "follow "
                    checked: true
                    checkable: true
                }
            }

            LabeledSlider {
                id: cameraAngle
                Layout.alignment: Qt.AlignRight
                from: 0
                to: 180.0
                value: 45.0
                text: "angle"
            }

            LabeledSlider {
                id: progressSlider
                from: 0
                to: missionSize
                enabled : missionReady
                text: (value / missionSize * 100).toLocaleString(Qt.locale(), 'f', 0) + "%"
                handleWidth: progressMetrics.width
                TextMetrics {
                    id: progressMetrics
                    font: progressSlider.font
                    text: "100%"
                }
            }

            LabeledSlider {
                id: animationSpeed
                Layout.alignment: Qt.AlignRight
                from: 1
                to: 100
                value: 50
                text: "speed"
            }

            Rectangle {
                id: mapFrame
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                Layout.minimumHeight: parent.height * 0.25
                Layout.minimumWidth: parent.width * 0.3
                color: "black"
                clip: true

                MapView {
                    id: mapView
                    objectName: "mapView"
                    anchors {
                        fill: parent
                        margins: 2
                    }

                    Map {
                        BasemapImagery { }
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
                        onPressed: mouse.accepted
                        onWheel: wheel.accepted
                    }
                }

                RowLayout {
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    spacing: 10

                    Rectangle {
                        Layout.margins: 5
                        height: width
                        width: childrenRect.width
                        clip: true
                        radius: 5

                        opacity: 0.9
                        Image {
                            source: "qrc:/Samples/Scenes/Animate3DSymbols/plus-16-f.png"
                            width: 24
                            height: width

                            MouseArea {
                                anchors.fill: parent
                                onClicked: zoomMapIn()
                            }
                        }
                    }

                    Rectangle {
                        Layout.margins: 5
                        height: width
                        width: childrenRect.width
                        opacity: 0.9
                        clip: true
                        radius: 5

                        Image {
                            source: "qrc:/Samples/Scenes/Animate3DSymbols/minus-16-f.png"
                            width: 24
                            height: width
                            MouseArea {
                                anchors.fill: parent
                                onClicked: zoomMapOut()
                            }
                        }
                    }
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
        cameraDistance: cameraDistance.value
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
        interval: 16.0 + 84 * (animationSpeed.to - animationSpeed.value) / 100.0;
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

        const fileName = missionName.replace(/\s/g, '') + ".csv";
        const fileContents = missionsFolder.readTextFile(fileName);
        const lines = fileContents.split("\n");
        for (let i = 0; i < lines.length; i++) {
            const dataParts = lines[i].split(",");
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
        const rtBldr = ArcGISRuntimeEnvironment.createObject("PolylineBuilder", {spatialReference: Factory.SpatialReference.createWgs84()});
        for (let j = 0; j < currentMissionModel.count; j++) {
            const missionData = currentMissionModel.get(j);
            rtBldr.addPointXY(missionData.lon, missionData.lat);
        }

        const firstData = currentMissionModel.get(0);
        const firstPos = createPoint(firstData);

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
            const missionData = currentMissionModel.get(progressSlider.value);
            const newPos = createPoint(missionData);

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
                        spatialReference: Factory.SpatialReference.createWgs84()
                    });
    }
}
