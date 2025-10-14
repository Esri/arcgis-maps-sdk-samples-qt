// Copyright 2018 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {

    signal saveMapClicked(var title, var tags, var description)
    signal cancelClicked()

    Pane {
        anchors.centerIn: parent
        background: Rectangle {
            color: "transparent"
            border.color: palette.shadow
            border.width: 1
            radius: 5
        }

        GridLayout {
            id: layerColumn
            columns: 2
            anchors {
                centerIn: parent
                margins: 16
            }

            Label {
                id: title
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.bottomMargin: 2
                text: "Title*:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: titleText
                selectByMouse: true
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                Layout.topMargin: 0
                placeholderText: "ex: Sample Map"
            }

            Label {
                id: tags
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.bottomMargin: 2
                text: "Tags:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: tagsText
                selectByMouse: true
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                Layout.topMargin: 0
                placeholderText: "ex: map, sample, elevation"
            }

            Label {
                id: description
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.bottomMargin: 2
                text: "Description:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: descriptionText
                selectByMouse: true
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                Layout.topMargin: 0
                placeholderText: "ex: This map displays..."
            }

            Label {
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "*Field is Required"
                color: "red"
                font {
                    pixelSize: 10
                    family: "helvetica"
                }
            }

            Button {
                Layout.margins: 5
                text: "Save"
                onClicked: {
                    // Make sure a Title is supplied
                    if (titleText.text === "") {
                        title.color = "red";
                        return;
                    }

                    saveMapClicked(titleText.text, tagsText.text, descriptionText.text)
                }
            }

            Button {
                Layout.margins: 5
                text: "Cancel"
                onClicked: cancelClicked()
            }
        }
    }
}
