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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Rectangle {
    RadialGradient {
        anchors.fill: parent
        opacity: 0.5
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.7; color: "black" }
        }
    }

    signal saveMapClicked(var title, var tags, var description)
    signal cancelClicked()

    Rectangle {
        color: "#edeeef"
        radius: 5
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
        border {
            color: "#77787a"
            width: 1
        }

        GridLayout {
            id: layerColumn
            columns: 2

            Text {
                id: title
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "Title*:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: titleText
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true

                placeholderText: "ex: Sample Map"
            }

            Text {
                id: tags
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "Tags:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: tagsText
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                placeholderText: "ex: map, sample, elevation"
            }

            Text {
                id: description
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "Description:"
                font {
                    pixelSize: 14
                    family: "helvetica"
                }
            }

            TextField {
                id: descriptionText
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                placeholderText: "ex: This map displays..."
            }

            Text {
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "*Field is Required"
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
