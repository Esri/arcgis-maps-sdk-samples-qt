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
    }

    Callout {
        id: callout
        calloutData: view.calloutData
        accessoryButtonVisible: false
        leaderPosition: Callout.LeaderPosition.Automatic
    }

    Button {
        id: clusterToggleButton
        width: 250
        height: 50
        text: qsTr("Toggle feature clustering")
        anchors {
            top: parent.top
            topMargin: 15
            right: parent.right
            rightMargin: 15
        }
        enabled: !model.taskRunning
        onClicked:
        {
            model.toggleClustering();
            callout.visible = false;
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: model.taskRunning
    }
}
