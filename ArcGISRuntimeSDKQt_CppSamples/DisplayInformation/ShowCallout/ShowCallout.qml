// [WriteFile Name=ShowCallout, Category=DisplayInformation]
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
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

ShowCalloutSample {
    id: showCalloutSample
    clip: true
    width: 800
    height: 600


    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        clip: true

        //! [set callout data]
        Callout {
            id: callout
            calloutData: showCalloutSample.calloutData // bind to the property that is exposed
            accessoryButtonHidden: true
            leaderPosition: leaderPositionEnum.Automatic
        }
        //! [set callout data]
    }
}
