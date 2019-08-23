// [WriteFile Name=DisplayDeviceLocation, Category=Maps]
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
import Esri.Samples 1.0

DisplayDeviceLocationSample {
    id: deviceLocationSample
    width: 800
    height: 600
    
    property string currentModeText: deviceLocationSample.stopMode
    property string currentModeImage: "qrc:/Samples/Maps/DisplayDeviceLocation/Stop.png"

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // populate list model with modes
            autoPanListModel.append({name: deviceLocationSample.compassMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/Compass.png"});
            autoPanListModel.append({name: deviceLocationSample.navigationMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/Navigation.png"});
            autoPanListModel.append({name: deviceLocationSample.recenterMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/Re-Center.png"});
            autoPanListModel.append({name: deviceLocationSample.onMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/On.png"});
            autoPanListModel.append({name: deviceLocationSample.stopMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/Stop.png"});
            autoPanListModel.append({name: deviceLocationSample.closeMode, image: "qrc:/Samples/Maps/DisplayDeviceLocation/Close.png"});
        }
    }

    Rectangle {
        id: rect
        anchors.fill: parent
        visible: autoPanListView.visible
        color: "black"
        opacity: 0.7
    }

    ListView {
        id: autoPanListView
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        visible: false
        width: parent.width
        height: 300
        spacing: 10
        model: ListModel {
            id: autoPanListModel
        }

        delegate: Row {
            id: autopanRow
            anchors.right: parent.right
            spacing: 10

            Text {
                text: name
                font.pixelSize: 25
                color: "white"
                MouseArea {
                    anchors.fill: parent
                    // When an item in the list view is clicked
                    onClicked: {
                       autopanRow.updateAutoPanMode();
                    }
                }
            }

            Image {
                source: image
                width: 40
                height: width
                MouseArea {
                    anchors.fill: parent
                    // When an item in the list view is clicked
                    onClicked: {
                       autopanRow.updateAutoPanMode();
                    }
                }
            }

            // set the appropriate auto pan mode
            function updateAutoPanMode() {
                switch (name) {
                case deviceLocationSample.compassMode:
                    deviceLocationSample.setAutoPanMode(deviceLocationSample.compassMode);
                    deviceLocationSample.startLocationDisplay();
                    break;
                case deviceLocationSample.navigationMode:
                    deviceLocationSample.setAutoPanMode(deviceLocationSample.navigationMode);
                    deviceLocationSample.startLocationDisplay();
                    break;
                case deviceLocationSample.recenterMode:
                    deviceLocationSample.setAutoPanMode(deviceLocationSample.recenterMode);
                    deviceLocationSample.startLocationDisplay();
                    break;
                case deviceLocationSample.onMode:
                    deviceLocationSample.setAutoPanMode(deviceLocationSample.onMode);
                    deviceLocationSample.startLocationDisplay();
                    break;
                case deviceLocationSample.stopMode:
                    deviceLocationSample.stopLocationDisplay();
                    break;
                }

                if (name !== deviceLocationSample.closeMode) {
                    currentModeText = name;
                    currentModeImage = image;
                }

                // hide the list view
                currentAction.visible = true;
                autoPanListView.visible = false;
            }
        }
    }

    Row {
        id: currentAction
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 10
            bottomMargin: 25
        }
        spacing: 10

        Text {
            text: currentModeText
            font.pixelSize: 25
            color: "white"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    currentAction.visible = false;
                    autoPanListView.visible = true;
                }
            }
        }

        Image {
            source: currentModeImage
            width: 40
            height: width
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    currentAction.visible = false;
                    autoPanListView.visible = true;
                }
            }
        }
    }
}
