// [WriteFile Name=ServiceArea, Category=RoutingAndNavigation]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Esri.Samples 1.0
//import Esri.ArcGISExtras 1.1

ServiceAreaSample {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property double scaleFactor: 1 //System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Column {
        anchors {
            fill: parent
            margins: 8 * scaleFactor
        }
        spacing: 16 * scaleFactor

        ExclusiveGroup {
            id: modeGroup
        }

        Button {
            id: facilitiesButton
            text: "Facility"
            checkable: true
            enabled: !busy
            exclusiveGroup: modeGroup
            width: serviceAreasButton.width
            iconSource: "qrc:/Samples/RoutingAndNavigation/ServiceArea/ic_menu_addencircled_light.png"

            onClicked: setFacilityMode();
        }

        Button {
            id: barrierButton
            text: "Barrier"
            checkable: true
            enabled: !busy
            exclusiveGroup: modeGroup
            width: serviceAreasButton.width
            iconSource: "qrc:/Samples/RoutingAndNavigation/ServiceArea/ic_menu_addencircled_light.png"

            onClicked: setBarrierMode();
        }

        Button {
            id: newBarrierButton
            text: "new barrier"
            anchors.right: barrierButton.right
            enabled: barrierButton.checked

            onClicked: newBarrier();
        }

        Button {
            id: serviceAreasButton
            text: "Service Area"
            checkable: true
            enabled: !busy
            exclusiveGroup: modeGroup
            iconSource: "qrc:/Samples/RoutingAndNavigation/ServiceArea/ic_menu_find_light.png"

            onClicked: setSolveServiceAreaMode();
        }

        Button {
            text: "Reset"
            width: serviceAreasButton.width
            enabled: !busy
            iconSource: "qrc:/Samples/RoutingAndNavigation/ServiceArea/ic_menu_closeclear_light.png"
            onClicked: {
                facilitiesButton.checked = false;
                barrierButton.checked = false;
                serviceAreasButton.checked = false;
                resetMode();
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: busy
    }

    MessageDialog {
        id: messageDialog
        title: "Route Error"
        text: message
        visible: text.length > 0
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
