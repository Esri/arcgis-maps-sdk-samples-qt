// [WriteFile Name=TokenAuthentication, Category=CloudAndPortal]
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
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    Portal {
        id: agolPortal
        url: "https://www.arcgis.com/"
    }

    PortalItem {
        id: trafficWebMap
        portal: agolPortal
        itemId: "e5039444ef3c48b8a8fdc9227f9be7c1"
    }

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            item: trafficWebMap
        }
    }

    // Uncomment this section when running as standalone application

    /*
    // Declare an AuthenticationView
    AuthenticationView {
        anchors.fill: parent
    }
    */
}
