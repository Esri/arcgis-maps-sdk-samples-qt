// [WriteFile Name=ShowPopup, Category=DisplayInformation]
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
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit 100.15

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

    PopupStackView {
        id: popupStackView
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        popupManagers: model.popupManagers
        visible: false

        onVisibleChanged: {
            if (!visible)
                model.clearSelection();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ShowPopupSample {
        id: model
        mapView: view

        onPopupManagersChanged: popupStackView.visible = true
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: model.taskRunning
    }
}
