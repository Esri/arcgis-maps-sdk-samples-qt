// [WriteFile Name=ClusterFeatures, Category=DisplayInformation]
// [Legal]
// Copyright 2023 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

Item {
    id: item1

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ClusterFeaturesSample {
        id: model
        mapView: view

        onPopupManagersChanged: popupStackView.visible = true
    }

    Button {
        id: clusterToggleButton
        width: 250
        height: 50
        text: qsTr("Toggle feature clustering")
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        enabled: !model.taskRunning
        onClicked:
        {
            model.toggleClustering();
            callout.visible = false;
        }
    }

    PopupStackView {
        id: popupStackView
        anchors {
            top: parent.top
            left: parent.left
            margins: 20
        }
        popupManagers: model.popupManagers
        visible: false
    }

    Rectangle {
        id: aggregateTextRect
        anchors {
            top: parent.top
            left: parent.left
            margins: 20
        }
        background.color: "white"
        Text {
            id: aggregateText
            anchors.fill: parent
            anchors.margins: 15
            text: "\n            This cluster represents <b>3</b> power plants with an average capacity of <b>219.50 megawatts</b>.\n             The power plants in this cluster produce a total of <b>658.5 megawatts</b> of power."
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: model.taskRunning
    }
}
