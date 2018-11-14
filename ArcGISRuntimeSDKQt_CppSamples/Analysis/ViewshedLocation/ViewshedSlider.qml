// [WriteFile Name=ViewshedLocation, Category=Analysis]
// [Legal]
// Copyright 2017 Esri.

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

Column {
    id: root
    width: parent.width
    spacing: 5

    property string titleText
    property real parameterValue
    property real minValue: 1
    property real maxValue: 179

    Text {
        width: 80
        text: titleText
        font.pixelSize: 14
    }

    Row {
        width: parent.width
        height: 25
        spacing: 5

        Slider {
            anchors.verticalCenter: parent.verticalCenter
            orientation: Qt.Horizontal
            from: minValue
            to: maxValue
            width: parent.width * 0.75
            value: parameterValue

            onValueChanged: {
                parameterValue = value;
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignRight
            text: Math.round(parameterValue)
            font.pixelSize: 14
        }
    }
}
