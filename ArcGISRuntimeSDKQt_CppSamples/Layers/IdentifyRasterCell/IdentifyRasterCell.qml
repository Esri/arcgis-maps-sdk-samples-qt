// [WriteFile Name=IdentifyRasterCell, Category=Layers]
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

import QtQuick 2.15
import QtQuick.Controls 2.15
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit 100.15.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            id: callout
            calloutData: view.calloutData
            implicitWidth: 300
            contentItem: Label {
                id: customComponentText
                text: view.calloutData.detail
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    IdentifyRasterCellSample {
        id: model
        mapView: view
    }
}
