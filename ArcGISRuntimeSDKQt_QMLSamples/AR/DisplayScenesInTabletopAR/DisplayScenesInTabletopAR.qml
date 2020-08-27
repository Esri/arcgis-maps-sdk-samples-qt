// [WriteFile Name=DisplayScenesInTabletopAR, Category=Scenes]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1
import Esri.ArcGISArToolkit 1.0
import Esri.Samples 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mspk"
    readonly property double sceneWidth: 800.0
    readonly property double tableTopWidth: 1.0
    property var philadelphiaScene: null

    PermissionsHelper {
        id: permissionsHelper
        onRequestFilesystemAccessCompleted: mspk.load();
    }

    ArcGISArView {
        id: arcGISArView
        anchors.fill: parent
        tracking: true
        sceneView: sceneView
    }

    SceneView {
        id: sceneView
        anchors.fill: parent

        onMouseClicked: {
            if (mspk.loadStatus !== Enums.LoadStatusLoaded || !arcGISArView.tracking || !philadelphiaScene)
                return;

            // Display the scene
            if (sceneView.scene !== philadelphiaScene) {
                sceneView.scene = philadelphiaScene;
                sceneView.scene.sceneViewTilingScheme = Enums.SceneViewTilingSchemeGeographic;
            }

            // Set the clipping distance for the scene.
            arcGISArView.clippingDistance = 400;

            // Set the surface opacity to 0.
            arcGISArView.sceneView.scene.baseSurface.opacity = 0.0;

            // Enable subsurface navigation. This allows you to look at the scene from below.
            arcGISArView.sceneView.scene.baseSurface.navigationConstraint = Enums.NavigationConstraintNone;

            // Set the initial transformation using the point clicked on the screen
            arcGISArView.setInitialTransformation(mouse.x, mouse.y);

            // Set the origin camera.
            arcGISArView.originCamera = philadelphiaCamera;

            // Set the translation factor based on the scene content width and desired physical size.
            arcGISArView.translationFactor = sceneWidth/tableTopWidth;
        }

        Rectangle {
            anchors {
                bottom: sceneView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#88ffffff" // transparent white
            radius: 5
            visible: sceneView.scene !== philadelphiaScene ? true : false

            Text {
                anchors.centerIn: parent
                padding: 2
                font.bold: true
                text: qsTr("Touch screen to place the tabletop scene...")
            }
        }
    }

    Component.onCompleted: {
        if (!permissionsHelper.fileSystemAccessGranted)
            permissionsHelper.requestFilesystemAccess();
        else
            mspk.load();
    }

    MobileScenePackage {
        id: mspk
        path: dataPath + "/philadelphia.mspk"

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            if (mspk.scenes.length < 1)
                return;

            // Obtain the first scene in the list of scenes
            philadelphiaScene = mspk.scenes[0];
        }

        onErrorChanged: {
            console.log("Mobile Scene Package Error: %1 %2".arg(error.message).arg(error.additionalMessage));
        }
    }

    Camera {
        id: philadelphiaCamera
        location: Point {
            //latitude
            y: 39.95787000283599
            //longitude
            x: -75.16996728256345
            //altitide
            z: 8.813445091247559
        }
        heading: 0
        pitch: 90
        roll:0
    }
}
