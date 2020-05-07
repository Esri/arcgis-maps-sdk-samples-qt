// [WriteFile Name=OpenMobileScenePackage, Category=Scenes]
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
import Esri.ArcGISRuntime 100.9
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/mspk"

    SceneView {
        id: sceneView
        anchors.fill: parent        
    }

    Component.onCompleted: {
        mspk.load();
    }

    MobileScenePackage {
        id: mspk
        path: dataPath + "/philadelphia.mspk"

        onLoadStatusChanged: {
            // only proceed once the scene package is loaded
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            if (mspk.scenes.length < 1)
                return;

            // obtain the first scene in the list of scenes
            const scene = mspk.scenes[0];

            // set the scene on the scene view to display
            sceneView.scene = scene;
        }

        onErrorChanged: {
            console.log("Mobile Scene Package Error: %1 %2".arg(error.message).arg(error.additionalMessage));
        }
    }
}
