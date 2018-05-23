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
import QtQuick.Controls 1.4
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
        anchors {
            fill: layerColumn
            margins: -15 * scaleFactor
        }

        color: "#edeeef"
        radius: 5 * scaleFactor
        border {
            color: "#77787a"
            width: 1 * scaleFactor
        }
    }

    Column {
        id: layerColumn
        anchors.centerIn: parent
        width: 200 * scaleFactor

        spacing: 10 * scaleFactor

        Text {
            id: title
            text: "Title*:"
            font {
                pixelSize: 14 * scaleFactor
                family: "helvetica"
            }
        }

        TextField {
            id: titleText
            width: parent.width
            placeholderText: "ex: Sample Map"
        }

        Text {
            id: tags
            text: "Tags:"
            font {
                pixelSize: 14 * scaleFactor
                family: "helvetica"
            }
        }

        TextField {
            id: tagsText
            width: parent.width
            placeholderText: "ex: map, sample, elevation"
        }

        Text {
            id: description
            text: "Description:"
            font {
                pixelSize: 14 * scaleFactor
                family: "helvetica"
            }
        }

        TextField {
            id: descriptionText
            width: parent.width
            placeholderText: "ex: This map displays..."
        }

        Text {
            text: "*Field is Required"
            font {
                pixelSize: 10 * scaleFactor
                family: "helvetica"
            }
        }

        Button {
            width: parent.width
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
            width: parent.width
            text: "Cancel"
            onClicked: cancelClicked()
        }
    }

    function reset() {
        titleText.text = "";
        descriptionText.text = "";
        tagsText.text = "";
    }
}
