// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
// [Legal]
// Copyright 2016 Esri.
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
import QtQuick.Effects
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

MobileMap_SearchAndRouteSample {
    id: mobileMapSearchRoute
    clip: true
    width: 800
    height: 600

    property bool isMapOpen: false

    Rectangle {
        anchors.fill: parent
        color: palette.base

        // add a mapView component
        MapView {
            id: mapView
            objectName: "mapView"

            Component.onCompleted: {
                // Set the focus on MapView to initially enable keyboard navigation
                forceActiveFocus();
            }

            // callout to display information
            Callout {
                calloutData: mapView.calloutData
                screenOffsetY: -19
                accessoryButtonVisible: false
                leaderPosition: Callout.LeaderPosition.Automatic
            }

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                }
                opacity: 0.50
                height: parent.height
                width: 25
                color: palette.base

                Rectangle {
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    width: parent.width
                    height: 100
                    color: palette.base

                    Image {
                        anchors.centerIn: parent
                        mirror: true
                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/chevron-right-24.svg"
                        height: 33
                        width: height
                        layer.enabled: true
                        layer.effect: MultiEffect {
                            colorization: 1.0
                            colorizationColor: palette.text
                            brightness: 1.0
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mapSelectionStack.pop();
                    }
                }
            }

            // Map controls
            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: mapView.attributionTop
                    margins: 10
                }
                spacing: 10

                Button {
                    id: routeButton
                    text: qsTr("Route")
                    leftPadding: 15
                    rightPadding: 15
                    width: 125
                    icon {
                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/road-sign-24.svg"
                        width: 24
                        height: 24
                        color: palette.buttonText
                    }
                    visible: mobileMapSearchRoute.canRoute
                    onClicked: {
                        mobileMapSearchRoute.solveRoute();
                    }
                }

                // clear graphics button
                Button {
                    id: clearButton
                    text: qsTr("Clear")
                    leftPadding: 15
                    rightPadding: 15
                    width: 125
                    icon {
                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/trash-24.svg"
                        width: 24
                        height: 24
                        color: palette.buttonText
                    }
                    visible: mobileMapSearchRoute.canClear
                    onClicked: {
                        mobileMapSearchRoute.resetMapView();
                    }
                }
            }
        }

        // use a StackView for the user interface
        StackView {
            id: mapSelectionStack
            anchors.fill: parent

            // begin with the mobile map package selection options
            initialItem: Item {

                Column {
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: parent.width
                    spacing: 20

                    // UI navigation bar
                    Rectangle {
                        width: parent.width
                        height: 100
                        color: palette.button
                        border.color: "darkgray"

                        // forward navigation button. Visible after first map is selected
                        Image {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                right: parent.right
                                margins: 10
                            }
                            source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/chevron-right-24.svg"
                            height: 44
                            width: height
                            visible: isMapOpen
                            layer.enabled: true
                            layer.effect: MultiEffect {
                                colorization: 1.0
                                colorizationColor: palette.buttonText
                                brightness: 1.0
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    mapSelectionStack.push(mapSelectView);
                                }
                            }
                        }

                        Label {
                            anchors.centerIn: parent
                            height: 40
                            font.pixelSize: 25
                            text: "Choose a Mobile Map Package"
                            color: "#F8F8F8"
                        }
                    }

                    // mobile map package ListView
                    ListView {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 400
                        width: 200
                        spacing: 10
                        model: mobileMapSearchRoute.mmpkList

                        delegate: Component {
                            Rectangle {

                                width: 200
                                height: 50
                                color: palette.button
                                radius: 2
                                border.color: "darkgray"

                                Label {
                                    anchors.centerIn: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    width: 150
                                    text: modelData
                                    color: "#F8F8F8"
                                    elide: Text.ElideMiddle
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    propagateComposedEvents: false
                                    onClicked: {
                                        isMapOpen = false;

                                        //generate list of maps and go to map selection view
                                        mobileMapSearchRoute.createMapList(index);
                                        mapSelectionStack.push(mapSelectView);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: mapSelectView

                Column {
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: parent.width
                    spacing: 20

                    // UI navigation bar
                    Rectangle {
                        width: parent.width
                        height: 100
                        color: palette.button
                        border.color: "darkgray"

                        // back button
                        Image {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                left: parent.left
                                margins: 10
                            }
                            mirror: true
                            source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/chevron-right-24.svg"
                            height: 44
                            width: height
                            layer.enabled: true
                            layer.effect: MultiEffect {
                                colorization: 1.0
                                colorizationColor: palette.buttonText
                                brightness: 1.0
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    mapSelectionStack.pop();
                                }
                            }
                        }

                        // forward button. Only visible after first map has been selected
                        Image {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                right: parent.right
                                margins: 10
                            }
                            source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/chevron-right-24.svg"
                            height: 44
                            width: height
                            visible: isMapOpen
                            layer.enabled: true
                            layer.effect: MultiEffect {
                                colorization: 1.0
                                colorizationColor: palette.buttonText
                                brightness: 1.0
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    mapSelectionStack.push(mapView);
                                }
                            }
                        }

                        Label {
                            anchors.centerIn: parent
                            height: 40
                            font.pixelSize: 25
                            text: "Choose a Map"
                            color: "#F8F8F8"
                        }
                    }

                    // list of maps
                    ListView {
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 400
                        width: 200
                        spacing: 10
                        model: mobileMapSearchRoute.mapList

                        delegate: Component {
                            Rectangle {
                                width: 200
                                height: 50
                                color: palette.button
                                radius: 2
                                border.color: "darkgray"

                                Label {
                                    anchors.centerIn: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    width: 150
                                    text: modelData.name
                                    elide: Text.ElideMiddle
                                    color: "#F8F8F8"
                                }

                                // geocoding available icon
                                Image {
                                    anchors {
                                        left: parent.left
                                        top: parent.top
                                        margins: 5
                                    }
                                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/pin-24.svg"
                                    height: 20
                                    width: height
                                    visible: modelData.geocoding
                                    layer.effect: MultiEffect {
                                        colorization: 1.0
                                        colorizationColor: palette.buttonText
                                        brightness: 1.0
                                    }
                                }

                                // routing available icon
                                Image {
                                    anchors {
                                        right: parent.right
                                        top: parent.top
                                        margins: 5
                                    }
                                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/road-sign-24.svg"
                                    height: 20
                                    width: height
                                    visible: modelData.routing
                                    layer.enabled: true
                                    layer.effect: MultiEffect {
                                        colorization: 1.0
                                        colorizationColor: palette.buttonText
                                        brightness: 1.0
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    propagateComposedEvents: false
                                    onClicked: {
                                        // set map and display mapView
                                        mobileMapSearchRoute.selectMap(index);
                                        isMapOpen = true;
                                        mapSelectionStack.push(mapView);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        BusyIndicator {
            anchors.centerIn: parent
            visible: mobileMapSearchRoute.isGeocodeInProgress
        }
    }
}
