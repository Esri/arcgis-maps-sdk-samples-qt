// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
// [Legal]
// Copyright 2022 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.13
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        id: locationInformationRectangle
        width: parent.width
        height: textColumn.height + 20
        color: "lightgray"

        ColumnLayout {
            id: textColumn
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }

            Text {
                text: "Initializing location data source\nand retrieving user location..."
                // Display if there are no location properties to display
                visible: Object.keys(model.locationProperties).length === 0;
            }

            Text {
                text: "Floor: " + model.locationProperties.floor
                visible: model.locationProperties.floor !== undefined
            }
            Text {
                text: "Position source: " + model.locationProperties.positionSource
                visible: model.locationProperties.positionSource !== undefined
            }
            Text {
                text: "Transmitter count: " + model.locationProperties.satelliteCount
                visible: model.locationProperties.satelliteCount !== undefined
            }
            Text {
                text: "Horizontal accuracy: " + (model.locationProperties.horizontalAccuracy ? model.locationProperties.horizontalAccuracy.toFixed(2) + " m" : "undefined")
                visible: model.locationProperties.horizontalAccuracy !== undefined
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ShowDeviceLocationUsingIndoorPositioningSample {
        id: model
        mapView: view
    }
}
