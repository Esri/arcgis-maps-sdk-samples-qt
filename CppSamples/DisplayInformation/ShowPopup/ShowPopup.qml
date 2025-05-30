// [WriteFile Name=ShowPopup, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.
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

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    PopupView {
        id: popupView
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        popup: model.popup

        visible: false

        onVisibleChanged: {
            if (!visible)
                model.clearSelection();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ShowPopupSample {
        id: model
        mapView: view

        onPopupChanged: popupView.visible = true
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: model.taskRunning
    }
}
