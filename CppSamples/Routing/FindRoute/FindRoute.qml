// [WriteFile Name=FindRoute, Category=Routing]
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
import Esri.Samples

FindRouteSample {
    id: findRouteSample
    width: 800
    height: 600

    onSolveRouteComplete: solveButton.visible = false

    Rectangle {
        id: directionWindow
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        visible: false
        property int targetWidth: Qt.platform.os === "ios" || Qt.platform.os === "android" ? 250 : 350
        width: visible ? targetWidth : 0
        color: palette.base
        clip: true
        Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutQuad } }

        //! [FindRoute cpp ListView directionsView]
        ListView {
            id: directionsView
            anchors {
                fill: parent
                margins: 5
            }
            header: Component {
                Label {
                    height: 40
                    text: qsTr("Directions:")
                    font.pixelSize: 22
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
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            right: directionWindow.left
        }

        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Create the solve button to solve the route
        Button {
            id: solveButton
            width: 130
            height: 30
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: mapView.attributionTop
                bottomMargin: 5
            }

            text: qsTr("Solve Route")
            onClicked: findRouteSample.solveRoute();
        }

        // Create a button to show the direction window
        Rectangle {
            id: directionButton

            property bool pressed: false

            visible: !solveButton.visible
            anchors {
                right: parent.right
                bottom: parent.bottom
                rightMargin: 10
                bottomMargin: 40
            }

            width: 45
            height: width
            color: pressed ? palette.base : palette.highlight
            radius: 100
            border {
                color: "#585858"
                width: 1.5
            }

            Image {
                anchors.centerIn: parent
                width: 35
                height: width
                source: "qrc:/Samples/Routing/FindRoute/directions.png"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: directionButton.pressed = true
                onReleased: directionButton.pressed = false
                onClicked: {
                    // Show the direction window when it is clicked
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
            height: 35
            color: directionWindow.color

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: -8
                    leftMargin: 20
                    rightMargin: 20
                }
                color: palette.dark
                height: 1
            }

            Label {
                text: directionText
                anchors {
                    fill: parent
                    leftMargin: 5
                }
                elide: Text.ElideRight
                font.pixelSize: 14
            }
        }
    }
}
