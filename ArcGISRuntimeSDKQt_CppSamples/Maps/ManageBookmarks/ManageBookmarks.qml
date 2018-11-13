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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

ManageBookmarksSample {
    id: manageBookmarksSample
    width: 800
    height: 600

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // Create the add button so new bookmarks can be added
        Rectangle {
            id: addButton
            property bool pressed: false
            anchors {
                right: parent.right
                bottom: mapView.attributionTop
                rightMargin: 20
                bottomMargin: 20
            }
            width: childrenRect.width
            height: childrenRect.height
            color: pressed ? "#959595" : "#D6D6D6"
            radius: 100
            border {
                color: "#585858"
                width: 1
            }

            Image {
                rotation: 45
                source: "qrc:/Samples/Maps/ManageBookmarks/add.png"
                width: 32
                height: 32
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
    }

    //! [Use BookmarkListModel in View]
    ComboBox {
        id: bookmarkComboBox
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        property int bestWidth: implicitWidth
        width: bestWidth + indicator.width + leftPadding + rightPadding
        // Set the model to the BookmarkListModel
        model: manageBookmarksSample.bookmarks

        onModelChanged: {
            var model = bookmarkComboBox.model;
            if (model)
            {
                var w = bestWidth;
                for (var i = 0; i < model.rowCount(); ++i) {
                    metrics.text = manageBookmarksSample.bookmarkNameForIndex(i);
                    w = Math.max(w, metrics.width);
                }
                bestWidth = w;
            }
        }

        onCurrentTextChanged: {
            // Call C++ invokable function to to go to the bookmark
            manageBookmarksSample.goToBookmark(bookmarkComboBox.currentIndex);
        }

        TextMetrics {
            id: metrics
            font: bookmarkComboBox.font
        }
    }
    //! [Use BookmarkListModel in View]


    // Create a window so names for new bookmarks can be specified
    Rectangle {
        id: addWindow
        visible: false
        enabled: visible
        color: "lightgrey"
        opacity: .9
        radius: 5
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
        border {
            color: "#4D4D4D"
            width: 1
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
        }

        GridLayout {
            columns: 2
            Text {
                text: qsTr("Provide the bookmark name")
                Layout.columnSpan: 2
                Layout.margins: 5
            }

            TextField {
                id: textField
                placeholderText: qsTr("ex: Grand Canyon")
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Cancel")
                onClicked: addWindow.visible = false;
                Layout.margins: 5
            }

            Button {
                text: qsTr("Done")
                Layout.margins: 5
                Layout.alignment: Qt.AlignRight
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
