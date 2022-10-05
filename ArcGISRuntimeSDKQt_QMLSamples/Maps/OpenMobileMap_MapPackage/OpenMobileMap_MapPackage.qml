// [WriteFile Name=OpenMobileMap_MapPackage, Category=Maps]
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

import QtQuick
import Esri.ArcGISRuntime
import Esri.ArcGISExtras

Rectangle {
    width: 800
    height: 600

    //! [open mobile map package qml api snippet]
    readonly property url dataPath: System.writableLocationPath(System.StandardPathsHomeLocation) + "/ArcGIS/Runtime/Data/mmpk/"

    // Create MapView
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Component.onCompleted: {
        mmpk.load();
    }

    // Create a Mobile Map Package and set the path
    MobileMapPackage {
        id: mmpk
        path: dataPath + "Yellowstone.mmpk"

        // wait for the mobile map package to load
        onLoadStatusChanged: {
            // only proceed once the map package is loaded
            if (loadStatus !== Enums.LoadStatusLoaded) {
                return;
            }

            if (mmpk.maps.length < 1) {
                return;
            }

            // set the map view's map to the first map in the mobile map package
            mapView.map = mmpk.maps[0];
        }

        onErrorChanged: {
            console.log("Mobile Map Package Error: %1 %2".arg(error.message).arg(error.additionalMessage));
        }
    }
    //! [open mobile map package qml api snippet]
}
