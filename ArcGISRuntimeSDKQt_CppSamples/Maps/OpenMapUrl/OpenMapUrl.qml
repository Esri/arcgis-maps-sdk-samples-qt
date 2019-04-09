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
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.Samples 1.0

OpenMapUrlSample {
    id: openMapUrlSample
    width: 800
    height: 600

    

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
    }

    // Create a list model with information about different webmaps
    ListModel {
       id: webmapsListModel
       ListElement { itemTitle: "Population Pressure"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/population.png"; itemId: "392451c381ad4109bf04f7bd442bc038"}
       ListElement { itemTitle: "USA Tapestry Segmentation"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/Tapestry.png"; itemId: "01f052c8995e4b9e889d73c3e210ebe3"}
       ListElement { itemTitle: "Geology of United States"; imageUrl: "qrc:/Samples/Maps/OpenMapUrl/geology.jpg"; itemId: "92ad152b9da94dee89b9e387dfe21acd"}
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
                    webmapsListView.currentIndex = index;
                   // Call C++ invokable function to open the map from a webmap Url using this item id
                    openMap(itemId);
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

           // Create a list view to display the items
           ListView {
               id: webmapsListView
               anchors {
                   fill: parent
                   margins: 10
               }
               // Assign the model to the list model of webmaps
               model: webmapsListModel
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
