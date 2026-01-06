// [WriteFile Name=ShowCallout, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

Item {
    function logSceneLocationVisibility() {
        const cd = viewSelector.currentIndex === 0
                ? mapView.calloutData
                : (viewSelector.currentIndex === 1
                   ? sceneView.calloutData
                   : localSceneView.calloutData);

        if (!cd) {
            console.log("CalloutData is null");
            return;
        }

        const v = cd.sceneLocationVisibility;
        console.log("CalloutData.sceneLocationVisibility (" + viewSelector.currentText + "):", v);
    }

    Connections {
        target: mapView.calloutData
        function onSceneLocationVisibilityChanged() {
            console.log("CalloutData.sceneLocationVisibility (MapView):", mapView.calloutData.sceneLocationVisibility);
        }
    }

    Connections {
        target: sceneView.calloutData
        function onSceneLocationVisibilityChanged() {
            console.log("CalloutData.sceneLocationVisibility (SceneView):", sceneView.calloutData.sceneLocationVisibility);
        }
    }

    Connections {
        target: localSceneView.calloutData
        function onSceneLocationVisibilityChanged() {
            console.log("CalloutData.sceneLocationVisibility (LocalSceneView):", localSceneView.calloutData.sceneLocationVisibility);
        }
    }

    // Create the ShowCallout sample
    ShowCalloutSample {
        id: model
        mapView: mapView
        sceneView: sceneView
        localSceneView: localSceneView
    }

    // add a MapView component
    MapView {
        id: mapView
        anchors.fill: parent
        clip: true
        visible: viewSelector.currentIndex === 0

        Component.onCompleted: {
            if (visible)
                forceActiveFocus();
        }

        Callout {
            calloutData: mapView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // add a SceneView component
    SceneView {
        id: sceneView
        anchors.fill: parent
        clip: true
        visible: viewSelector.currentIndex === 1

        Component.onCompleted: {
            if (visible)
                forceActiveFocus();
        }

        Callout {
            calloutData: sceneView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // add a LocalSceneView component
    LocalSceneView {
        id: localSceneView
        anchors.fill: parent
        clip: true
        visible: viewSelector.currentIndex === 2

        Component.onCompleted: {
            if (visible)
                forceActiveFocus();
        }

        Callout {
            calloutData: localSceneView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // Display instructions
    Rectangle {
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }
        width: Math.max(instructionText.implicitWidth, viewSelector.implicitWidth) + 40
        height: instructionText.implicitHeight + viewSelector.implicitHeight + 40
        color: "#ffffff"
        opacity: 0.8
        radius: 5

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            ComboBox {
                id: viewSelector
                model: ["MapView", "SceneView", "LocalSceneView"]

                onCurrentIndexChanged: {
                    if (currentIndex === 0)
                        mapView.forceActiveFocus();
                    else if (currentIndex === 1)
                        sceneView.forceActiveFocus();
                    else
                        localSceneView.forceActiveFocus();

                    logSceneLocationVisibility();
                }
            }

            Text {
                id: instructionText
                text: viewSelector.currentIndex === 0
                      ? "Click anywhere in the map to show a callout"
                      : (viewSelector.currentIndex === 1
                         ? "Click anywhere in the scene to show a callout"
                         : "Click anywhere in the local scene to show a callout")
                font.pixelSize: 14
                wrapMode: Text.Wrap
            }
        }
    }
}
