// Copyright 2015 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

ManageBookmarksSample {
    id: manageBookmarksSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    ComboBox {
        id: bookmarks
        anchors {
            left: parent.left
            top: parent.top
            margins: 15 * scaleFactor
        }
        width: 225 * scaleFactor
        // Set the model equal to the QStringList created in C++
        model: manageBookmarksSample.bookmarkList

        onCurrentTextChanged: {
            // Call C++ invokable function to to go to the bookmark
            manageBookmarksSample.goToBookmark(bookmarks.currentText);
        }
    }

    // Create the add button so new bookmarks can be added
    Rectangle {
        id: addButton
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20 * scaleFactor
            bottomMargin: 40 * scaleFactor
        }

        width: 45 * scaleFactor
        height: width
        color: "#D6D6D6"
        radius: 100
        layer.enabled: true
        layer.effect: Glow {
            radius: 1.5
            samples: radius * 2
            source: addButton
            color: "#585858"
            transparentBorder: true
        }

        Image {
            anchors.centerIn: parent
            rotation: 45
            width: 45 * scaleFactor
            height: width
            source: "qrc:/Samples/Maps/ManageBookmarks/add.png"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Show the add window when it is clicked
                addWindow.visible = true;
            }
        }
    }

    // Create a window so names for new bookmarks can be specified
    Rectangle {
        id: addWindow
        anchors.centerIn: parent
        width: 225 * scaleFactor
        height: 100 * scaleFactor
        visible: false
        color: "lightgrey"
        opacity: .9
        radius: 5
        border {
            color: "#4D4D4D"
            width: 1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
        }

        Column {
            anchors.centerIn: parent
            spacing: 5
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: qsTr("Provide the bookmark name")
                    font.pixelSize: 12 * scaleFactor
                }
            }

            TextField {
                id: textField
                width: 180 * scaleFactor
                placeholderText: qsTr("ex: Grand Canyon")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 5
                Button {
                    width: (textField.width / 2) - (2.5 * scaleFactor)
                    text: "Cancel"
                    onClicked: addWindow.visible = false;
                }
                Button {
                    width: (textField.width / 2) - (2.5 * scaleFactor)
                    text: qsTr("Done")
                    onClicked: {
                        // Call the C++ invokable function and pass in the name specified
                        manageBookmarksSample.addBookmark(textField.text);
                        bookmarks.currentIndex = bookmarkList.length - 1;
                        textField.text = "";
                        addWindow.visible = false;
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


