// [WriteFile Name=MobileMap_SearchAndRoute, Category=Maps]
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
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1
import Esri.ArcGISRuntime.Toolkit.Controls 2.0


MobileMap_SearchAndRouteSample {
    id: mobileMapSearchRoute
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property bool isMapOpen: false

    // add a mapView component
    MapView {
        id: mapView
        objectName: "mapView"
        visible: false

        Callout

        // back button
        Image {
            anchors {
                top: parent.top
                left: parent.left
                margins: 10 * scaleFactor
            }
            source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/back.png"
            height: 44 * scaleFactor
            width: height

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mapSelectionStack.pop();
                }
            }
        }
    }

    StackView {
        id: mapSelectionStack
        anchors.fill: parent

        initialItem: Item {

            Column {

                anchors {
                    top: parent.top
                    left: parent.left
                }
                width: parent.width

                spacing: 20 * scaleFactor

                Rectangle {
                    width: parent.width
                    height: 100 * scaleFactor
                    color: "#283593"

                    Image {
                        anchors {
                            top: parent.top
                            right: parent.right
                            margins: 10 * scaleFactor
                        }

                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/forwardIcon.png"
                        height: 44 * scaleFactor
                        width: height
                        visible: isMapOpen

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                mapSelectionStack.push(mapSelectView);
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        height: 40 * scaleFactor
                        font.pixelSize: 25 * scaleFactor
                        text: "Choose a Mobile Map Package"
                        renderType: Text.NativeRendering
                    }
                }

                ListView {
                    id: mmpkListView
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 400 * scaleFactor
                    width: 200 * scaleFactor
                    spacing: 10 * scaleFactor
                    model: mobileMapSearchRoute.mmpkList

                    delegate: Component {
                        Rectangle {

                            width: 200 * scaleFactor
                            height: 50 * scaleFactor
                            color: "transparent"
                            radius: 2
                            border.color: "#283593"

                            Text {
                                anchors.centerIn: parent
                                horizontalAlignment: Text.AlignHCenter
                                width: 150 * scaleFactor
                                text: modelData
                                renderType: Text.NativeRendering
                                elide: Text.ElideMiddle
                            }

                            MouseArea {
                                anchors.fill: parent
                                propagateComposedEvents: false
                                onClicked: {
                                    isMapOpen = false;
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
                spacing: 20 * scaleFactor

                Rectangle {
                    width: parent.width
                    height: 100 * scaleFactor
                    color: "#283593"

                    // back button
                    Image {
                        anchors {
                            top: parent.top
                            left: parent.left
                            margins: 10 * scaleFactor
                        }
                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/back.png"
                        height: 44 * scaleFactor
                        width: height

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                mapSelectionStack.pop();
                            }
                        }
                    }

                    // forward button
                    Image {
                        anchors {
                            top: parent.top
                            right: parent.right
                            margins: 10 * scaleFactor
                        }

                        source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/forwardIcon.png"
                        height: 44 * scaleFactor
                        width: height
                        visible: isMapOpen

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                mapSelectionStack.push(mapView);
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        height: 40 * scaleFactor
                        font.pixelSize: 25 * scaleFactor
                        text: "Choose a Map"
                        renderType: Text.NativeRendering
                    }
                }

                ListView {
                    id: mapListView
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 400 * scaleFactor
                    width: 200 * scaleFactor
                    spacing: 10 * scaleFactor
                    model: mobileMapSearchRoute.mapList

                    delegate: Component {
                        Rectangle {

                            width: 200 * scaleFactor
                            height: 50 * scaleFactor
                            color: "transparent"
                            radius: 2
                            border.color: "#283593"

                            Text {
                                anchors.centerIn: parent
                                horizontalAlignment: Text.AlignHCenter
                                width: 150 * scaleFactor
                                text: modelData.name
                                renderType: Text.NativeRendering
                                elide: Text.ElideMiddle
                            }

                            // geocoding available icon
                            Image {
                                anchors {
                                    left: parent.left
                                    top: parent.top
                                    margins: 5 * scaleFactor
                                }
                                source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/pinOutlineSymbol.png"
                                height: 20 * scaleFactor
                                width: height
                                visible: modelData.geocoding
                            }

                            // routing available icon
                            Image {
                                anchors {
                                    right: parent.right
                                    top: parent.top
                                    margins: 5 * scaleFactor
                                }
                                source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/routingSymbol.png"
                                height: 20 * scaleFactor
                                width: height
                                visible: modelData.routing
                            }

                            MouseArea {
                                anchors.fill: parent
                                propagateComposedEvents: false
                                onClicked: {
                                    // set map
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

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
