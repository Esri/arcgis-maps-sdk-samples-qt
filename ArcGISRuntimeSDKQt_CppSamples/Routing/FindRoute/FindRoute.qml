// [WriteFile Name=FindRoute, Category=Routing]
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
import QtQuick.Window 2.2
import Esri.Samples 1.0

FindRouteSample {
    id: findRouteSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    onSolveRouteComplete: solveButton.visible = false

    // Create window for displaying the route directions
    Rectangle {
        id: directionWindow
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        visible: false
        width: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 250 * scaleFactor : 350 * scaleFactor
        color: "#FBFBFB"

        //! [FindRoute cpp ListView directionsView]
        ListView {
            id: directionsView
            anchors {
                fill: parent
                margins: 5 * scaleFactor
            }
            header: Component {
                Text {
                    height: 40 * scaleFactor
                    text: "Directions:"
                    font.pixelSize: 22 * scaleFactor
                }
            }

            // set the model to the DirectionManeuverListModel returned from the route
            model: findRouteSample.directions
            delegate: directionDelegate
        }
        //! [FindRoute cpp ListView directionsView]
    }

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // set the transform to animate showing the direction window
        transform: Translate {
            id: translate
            x: 0
            Behavior on x { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }
        }

        // Create the solve button to solve the route
        Rectangle {
            id: solveButton
            property bool pressed: false
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                bottomMargin: 5 * scaleFactor
            }

            width: 130 * scaleFactor
            height: 30 * scaleFactor
            color: pressed ? "#959595" : "#D6D6D6"
            radius: 5
            border {
                color: "#585858"
                width: 1 * scaleFactor
            }

            Text {
                id: routeButtonText
                anchors.centerIn: parent
                text: "Solve route"
                font.pixelSize: 14 * scaleFactor
                color: "#35352E"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: solveButton.pressed = true
                onReleased: solveButton.pressed = false
                onClicked: {
                    findRouteSample.solveRoute();
                }
            }
        }

        // Create a button to show the direction window
        Rectangle {
            id: directionButton

            property bool pressed: false

            visible: !solveButton.visible
            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 10 * scaleFactor
                bottomMargin: 40 * scaleFactor
            }

            width: 45 * scaleFactor
            height: width
            color: pressed ? "#959595" : "#D6D6D6"
            radius: 100
            border {
                color: "#585858"
                width: 1.5 * scaleFactor
            }

            Image {
                anchors.centerIn: parent
                width: 35 * scaleFactor
                height: width
                source: "qrc:/Samples/Routing/FindRoute/directions.png"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: directionButton.pressed = true
                onReleased: directionButton.pressed = false
                onClicked: {
                    // Show the direction window when it is clicked
                    translate.x = directionWindow.visible ? 0 : (directionWindow.width * -1);
                    directionWindow.visible = !directionWindow.visible;
                }
            }
        }
    }

    Component {
        id: directionDelegate
        Rectangle {
            id: rect
            width: parent.width
            height: 35 * scaleFactor
            color: directionWindow.color

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8 * scaleFactor
                    leftMargin: 20 * scaleFactor
                    rightMargin: 20 * scaleFactor
                }
                color: "darkgrey"
                height: 1 * scaleFactor
            }

            Text {
                text: directionText
                anchors {
                    fill: parent
                    leftMargin: 5 * scaleFactor
                }
                elide: Text.ElideRight
                font.pixelSize: 14 * scaleFactor
            }
        }
    }
}
