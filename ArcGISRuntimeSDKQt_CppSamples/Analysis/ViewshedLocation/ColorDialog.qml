// [WriteFile Name=ViewshedLocation, Category=Analysis]
// [Legal]
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
// [Legal]

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Dialog {
    id: colorDialog
    property alias red: redSlider.value
    property alias green: greenSlider.value
    property alias blue: blueSlider.value
    property alias alpha: alphaSlider.value
    readonly property color color: Qt.rgba(red, green, blue, alpha);

    title: "Pick a color"
    standardButtons: Dialog.Ok | Dialog.Cancel

    function setColor(c) {
        red = c.r;
        green = c.g;
        blue = c.b;
        alpha = c.a;
    }

    GridLayout {
        columns: 2
        Rectangle {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            height: redSlider.height
            border {
                color: "black"
                width: 1
            }
            radius: 2
            color: colorDialog.color
        }

        Text {
            text: "Red"
        }

        Slider {
            id: redSlider
        }

        Text {
            text: "Green"
        }

        Slider {
            id: greenSlider
        }

        Text {
            text: "Blue"
        }

        Slider {
            id: blueSlider
        }

        Text {
            text: "Alpha"
        }

        Slider {
            id: alphaSlider
        }
    }
}
