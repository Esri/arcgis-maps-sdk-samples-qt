// [WriteFile Name=ManageBookmarks, Category=Maps]
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

ManageBookmarksSample {
    id: manageBookmarksSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" ? 96 : 72)

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    //! [Use BookmarkListModel in View]
    ComboBox {
        id: bookmarkComboBox
        anchors {
            left: parent.left
            top: parent.top
            margins: 15 * scaleFactor
        }
        width: 225 * scaleFactor
        // Set the model to the BookmarkListModel
        model: manageBookmarksSample.bookmarks

        onCurrentTextChanged: {
            // Call C++ invokable function to to go to the bookmark
            manageBookmarksSample.goToBookmark(bookmarkComboBox.currentIndex);
        }
    }
    //! [Use BookmarkListModel in View]

    // Create the add button so new bookmarks can be added
    Rectangle {
        id: addButton
        property bool pressed: false
        anchors {
            right: parent.right
            bottom: parent.bottom
            rightMargin: 20 * scaleFactor
            bottomMargin: 40 * scaleFactor
        }

        width: 45 * scaleFactor
        height: width
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 100
        border {
            color: "#585858"
            width: 1 * scaleFactor
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
            onPressed: addButton.pressed = true
            onReleased: addButton.pressed = false
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
            width: 1 * scaleFactor
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
                    text: qsTr("Cancel")
                    onClicked: addWindow.visible = false;
                }
                Button {
                    width: (textField.width / 2) - (2.5 * scaleFactor)
                    text: qsTr("Done")
                    onClicked: {
                        // Call the C++ invokable function and pass in the name specified
                        manageBookmarksSample.addBookmark(textField.text);
                        bookmarkComboBox.currentIndex = bookmarkComboBox.count - 1;
                        textField.text = "";
                        addWindow.visible = false;
                    }
                }
            }
        }
    }
}
