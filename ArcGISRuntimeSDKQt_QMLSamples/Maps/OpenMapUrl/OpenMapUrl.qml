// [WriteFile Name=OpenMapUrl, Category=Maps]
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
import QtGraphicalEffects 1.0
import Esri.ArcGISRuntime

Rectangle {
    width: 800
    height: 600

    // Create a MapView
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Create an initial Map with the Imagery basemap
        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISImageryStandard
            }
        }
    }

    // Create a list model with information about different webmaps
    ListModel {
        id: webmapListModel
        ListElement { itemTitle: "Geology of United States"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/geology.jpg"; itemId: "92ad152b9da94dee89b9e387dfe21acd"}
        ListElement { itemTitle: "Terrestrial Ecosystems of the World"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/ecosystems.png"; itemId: "5be0bc3ee36c4e058f7b3cebc21c74e6"}
        ListElement { itemTitle: "Recent Hurricanes, Cyclones, and Typhoons"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/traces.png"; itemId: "064f2e898b094a17b84e4a4cd5e5f549"}
    }

    // Create a delegate for how the webmaps display in the view
    Component {
        id: webmapsDelegate
        Item {
            width: parent.width
            height: 65

            Row {
                spacing: 10
                Image {
                    source: imageUrl
                    width: 100
                    height: 65
                }
                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        width: 100
                        text: itemTitle
                        wrapMode: Text.WordWrap
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                // When an item in the list view is clicked
                onClicked: {
                    webmapsListView.currentIndex = index

                    //! [Construct map from a webmap url]
                    // construct the webmap Url using the itemId
                    const organizationPortalUrl = "https://arcgis.com/sharing/rest/content/items/";
                    const webmapUrl = organizationPortalUrl + itemId;
                    // Create a new map and assign it the initUrl
                    const newMap = ArcGISRuntimeEnvironment.createObject("Map", {initUrl: webmapUrl});
                    //! [Construct map from a webmap url]

                    // Set the map to the MapView
                    mapView.map = newMap;
                    mapPickerWindow.visible = false;
                }
            }
        }
    }

    // Create a window to display the different webmaps that can be selected
    Rectangle {
        id: mapPickerWindow
        anchors.fill: parent
        color: "transparent"

        RadialGradient {
            anchors.fill: parent
            opacity: 0.7
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 250
            height: 200
            color: "lightgrey"
            opacity: .8
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1
            }

            // Create a list view to display the webmaps
            ListView {
                id: webmapsListView
                anchors {
                    fill: parent
                    margins: 10
                }
                // Assign the model to the list model of webmaps
                model: webmapListModel
                // Assign the delegate to the delegate created above
                delegate: webmapsDelegate
                spacing: 10
                clip: true
                highlightFollowsCurrentItem: true
                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                focus: true
            }
        }
    }

    // Create a button to show the map picker window
    Rectangle {
        id: switchButton
        property bool pressed: false
        visible: !mapPickerWindow.visible
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20
            bottomMargin: 40
        }

        width: 45
        height: width
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 100
        border {
            color: "#585858"
            width: 1
        }

        Image {
            anchors.centerIn: parent
            width: 35
            height: width
            source: "qrc:/Samples/Maps/OpenMapUrl/SwitchMap.png"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: switchButton.pressed = true
            onReleased: switchButton.pressed = false
            onClicked: {
                // Show the add window when it is clicked
                mapPickerWindow.visible = true;
            }
        }
    }
}
