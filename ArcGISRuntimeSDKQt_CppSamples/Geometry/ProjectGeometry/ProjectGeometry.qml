// [WriteFile Name=ProjectGeometry, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.

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
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

ProjectGeometrySample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // Declare a callout
        Callout {
            id: callout
            calloutData: rootRectangle.calloutData
            accessoryButtonHidden: true
            autoAdjustWidth: true
            maxWidth: 350
            leaderPosition: leaderPositionEnum.Automatic
        }
    }

    onCalloutDataChanged: callout.showCallout();
}
