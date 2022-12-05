// [WriteFile Name=IdentifyKmlFeatures, Category=Layers]
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

        Callout {
            id: callout
            calloutData: view.calloutData
            implicitWidth: 150
            implicitHeight: contentText.implicitHeight + (contentText.implicitHeight * .05)
            contentItem: Label {
                id: contentText
                text: model.calloutText
                wrapMode: Text.WordWrap
                textFormat: Text.RichText
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    IdentifyKmlFeaturesSample {
        id: model
        mapView: view
    }
}
