// [WriteFile Name=ConfigureClusters, Category=Layers]
// [Legal]
// Copyright 2023 Esri.
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
import QtQuick.Layouts
import Esri.ArcGISRuntime.Toolkit

Item {
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
    ConfigureClustersSample {
        id: sample
        mapView: view

        onPopupChanged: popupView.visible = true
    }

    // Add a background to the column
    Rectangle {
        anchors.fill: dialog
        radius: 10
        border.width: 1
        visible: Qt.platform.os === "android" || Qt.platform.os === "ios" ? !popupView.visible : true
    }

    PopupView {
        id: popupView
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        popup: sample.popup

        visible: false
        onVisibleChanged: {
            if (!visible)
                sample.clearSelection();
        }
    }

    Column {
        id: dialog
        spacing: 15
        padding: 10

        Button {
            id: button
            visible: !button.checked
            width: 200
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Draw clusters"
            checkable: true
            onClicked: {
                sample.drawClusters();
                sample.displayLabels(true);
            }
        }

        Label {
            visible: button.checked
            text: "Clustering properties"
            font.pointSize: 14
            padding: 5
        }

        CheckBox {
            visible: button.checked
            text: "Display Labels"
            checked: true
            onCheckedChanged: sample.displayLabels(checked);
        }

        Label {
            visible: button.checked
            text: "Cluster radius: " + Math.round(clusterRadiusSlider.value);
            font.pointSize: 14
            padding: 5
        }

        Slider {
            id: clusterRadiusSlider
            visible: button.checked
            from: 30
            to: 85
            value: 60
            onValueChanged: sample.setClusterRadius(value);
        }

        Label {
            visible: button.checked
            text: "MaxScale: " + Math.round(maxScaleSlider.value)
            font.pointSize: 14
            padding: 5
        }

        Slider {
            id: maxScaleSlider
            visible: button.checked
            from: 0
            to: 150000
            value: 0
            onValueChanged: sample.setMaxScale(value);
        }

        Label {
            visible: button.checked
            text: "Current map scale: " + Math.round(sample.mapScale);
            font.pointSize: 14
            padding: 5
        }
        visible: Qt.platform.os === "android" || Qt.platform.os === "ios" ? !popupView.visible : true
    }
}
