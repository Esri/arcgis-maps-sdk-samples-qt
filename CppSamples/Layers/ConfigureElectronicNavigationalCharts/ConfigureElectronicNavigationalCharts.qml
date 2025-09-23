// [WriteFile Name=AddEncExchangeSet, Category=Layers]
// [Legal]
// Copyright 2025 Esri.
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

Item {
    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Add the missing Callout component
        Callout {
            calloutData: mapView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ConfigureElectronicNavigationalChartsSample {
        id: chartsSample
        mapView: mapView
    }

    // Dim background when popup open
    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: settingsPopup.opened ? 0.30 : 0
        visible: settingsPopup.opened
        MouseArea {
            anchors.fill: parent
            onClicked: settingsPopup.close()
        }
    }

    SettingsPopup{
        id:settingsPopup
    }

    // Bottom toolbar
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.max(parent.height * 0.08, 50)
        border.width: 1

        // Settings toggle button
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            onClicked: settingsPopup.opened ? settingsPopup.close() : settingsPopup.open()
            contentItem: Text {
                text: qsTr("Display Settings")
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
