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


MobileMap_SearchAndRouteSample {
    id: mobileMapSearchRoute
    clip: true

    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    Rectangle {
        id: mapSelectionWindow
        anchors.fill: parent
        color: "transparent"

        RadialGradient {
            anchors.fill: parent
            opacity: 0.45
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.5; color: "black" }
            }
        }

        // create window for displaying available mmpk/maps
        Rectangle {
            id: selectionMenu
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                margins: 5 * scaleFactor
            }
            visible: true
            height: parent.height
            width: 275 * scaleFactor
            color: "#E0E0E0"
            radius: 1

            Column {
                anchors {
                    top: parent.top
                    left: parent.left
                }
                width: parent.width

                Rectangle {
                    width: parent.width
                    height: 100 * scaleFactor
                    color: "#283593"

                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        height: 40 * scaleFactor
                        font.pixelSize: 18 * scaleFactor
                        text: mapListView.state === "choosePackage" ? "Choose a Mobile Map Package" : "Choose a Map"
                        renderType: Text.NativeRendering
                    }
                }

                spacing: 100 * scaleFactor

                // displays either the avaialble MMPKs or maps
                ListView {
                    id: mapListView
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 400 * scaleFactor
                    width: 200 * scaleFactor
                    spacing: 10 * scaleFactor
                    model: mobileMapSearchRoute.mmpkProperties
                    state: "choosePackage"

                    states: [
                        State {
                            name: "choosePackage"
                            PropertyChanges { target: mapListView; model: mobileMapSearchRoute.mmpkProperties }
                        },

                        State {
                            name: "chooseMap"
                            PropertyChanges { target: mapListView; model: mobileMapSearchRoute.mmpkProperties }
                        }
                    ]

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
                                visible: mapListView.state === "chooseMap" && mobileMapList[selectedMmpkIndex].locatorTask !== null
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
                                visible: mapListView.state === "chooseMap" && routing
                            }

                            MouseArea {
                                anchors.fill: parent
                                propagateComposedEvents: false
                                onClicked: {
                                    if (mapListView.state === "choosePackage") {

                                        // reset map list
                                        mapsInBundle.clear();

                                        // create the list of maps within a package
                                        for(var i = 0; i < mobileMapList[index].maps.length; i++) {
                                            var mapTitle = mobileMapList[index].maps[i].item.title;

                                            mapTitle += " " + (i + 1);

                                            // add to ListModel
                                            mapsInBundle.append({"name": mapTitle, "routing": mobileMapList[index].maps[i].transportationNetworks.length > 0});
                                        }

                                        selectedMmpkIndex = index;
                                        mapListView.state = "chooseMap";
                                    }

                                    // if mobile map package has been selected, display map selection options
                                    else if (mapListView.state === "chooseMap") {
                                        selectedMapInBundleIndex = index;
                                        mapSelectionWindow.visible = false;
                                        mapView.visible = true;

                                        // set map
                                        mapView.map = mobileMapList[selectedMmpkIndex].maps[index];

                                        // animate map back to original position
                                        translate.x = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //backbutton
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    margins: 10 * scaleFactor
                }

                color: "#E0E0E0"
                height: 50 * scaleFactor
                width: height
                border.color: "black"
                radius: 2
                opacity: 0.90
                visible: mapListView.state !== "choosePackage"

                Image {
                    anchors {
                        centerIn: parent
                        margins: 5 * scaleFactor
                    }
                    source: "qrc:/Samples/Maps/MobileMap_SearchAndRoute/back.png"
                    height: 44 * scaleFactor
                    width: height
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (mapSelectionWindow.visible === true) {
                            // go from "choose map" to "choose mobile map package" display
                            if (mapListView.state === "chooseMap") {
                                // go back to MobileMapPackage selection
                                mapListView.state = "choosePackage";
                            }
                        }

                        // if MapView is currently in foreground, pop up map selection window
                        else {
                            mapSelectionWindow.visible = true;
                            mapListView.state = "chooseMap";
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
