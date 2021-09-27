// [WriteFile Name=ChangeBasemap, Category=Maps]
// [Legal]
// Copyright 2016 Esri.

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
import Esri.ArcGISRuntime 100.13
import Esri.ArcGISRuntime.Toolkit 100.13

Rectangle {
    width: 800
    height: 600

    // Create MapView that contains a Map
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            id: map
            // Set the initial basemap to Topographic
            Basemap {
                id: basemap
                initStyle: Enums.BasemapStyleArcGISTopographic
            }
        }

        Button {
            id: changeBasemapButton
            anchors {
                horizontalCenter: view.horizontalCenter
                bottom: view.attributionTop
                bottomMargin: 10
            }
            text: "Change Basemap"
            checkable: true
        }

        BasemapGallery {
            id: basemapGallery
            anchors.centerIn: view
            geoModel: map
            visible: changeBasemapButton.checked;
            onCurrentBasemapChanged: {
                changeBasemapButton.checked = false;
            }
        }
    }
}
