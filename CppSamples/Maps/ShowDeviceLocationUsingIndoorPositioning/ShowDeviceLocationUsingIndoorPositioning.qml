// [WriteFile Name=ShowDeviceLocationUsingIndoorPositioning, Category=Maps]
// [Legal]
// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import QtQuick.Dialogs

Item {
    function displayFloor(floor) {
        return floor >= 0 ? floor + 1 : floor;
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Component.onDestruction: {
            model.stopLocationDisplay();
        }
    }

    Rectangle {
        id: locationInformationRectangle
        width: parent.width
        height: textColumn.height + 20
        color: palette.base

        ColumnLayout {
            id: textColumn
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }

            Label {
                text: qsTr("Loading indoor data...")
                visible: model.isLoading
            }

            Label {
                text: model.locationProperties.floor !== undefined
                    ? qsTr("Current floor: ") + displayFloor(model.locationProperties.floor)
                    : ""
                visible: !model.isLoading && model.locationProperties.floor !== undefined
            }
            Label {
                text: model.locationProperties.horizontalAccuracy !== undefined
                    ? qsTr("Accuracy: ") + model.locationProperties.horizontalAccuracy.toFixed(2) + qsTr(" m")
                    : ""
                visible: !model.isLoading && model.locationProperties.horizontalAccuracy !== undefined
            }
            Label {
                text: qsTr("Data source: ") + (model.locationProperties.positionSource !== undefined
                    ? model.locationProperties.positionSource
                    : qsTr("None"))
                visible: !model.isLoading
            }
            Label {
                text: qsTr("Number of Transmitters: ") + model.locationProperties.transmitterCount
                visible: !model.isLoading && model.locationProperties.transmitterCount !== undefined
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: model.isLoading
        visible: model.isLoading
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ShowDeviceLocationUsingIndoorPositioningSample {
        id: model
        mapView: view
    }

    Connections {
        target: model
        function onLocationPermissionDenied() {
            locationPermissionDeniedDialog.open()
        }
    }

    Dialog {
        id: locationPermissionDeniedDialog
        title: qsTr("Location Permission Denied")
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            text: qsTr("This application requires location permission.")
        }
    }

    Connections {
        target: model
        function onBluetoothPermissionDenied() {
            bluetoothPermissionDeniedDialog.open()
        }
    }

    Connections {
        target: model
        function onErrorOccurred(errorMessage) {
            setupErrorLabel.text = errorMessage
            setupErrorDialog.open()
        }
    }

    Dialog {
        id: bluetoothPermissionDeniedDialog
        title: qsTr("Bluetooth Permission Denied")
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            text: qsTr("This application requires bluetooth permission.")
        }
    }

    Dialog {
        id: setupErrorDialog
        title: qsTr("Indoor Positioning Error")
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Label {
            id: setupErrorLabel
            text: ""
        }
    }
}
