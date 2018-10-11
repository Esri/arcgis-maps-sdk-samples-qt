// [WriteFile Name=RasterRgbRenderer, Category=Layers]
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

Row {
    property bool isMin
    property double maxRange
    property alias numVals: repeater.model

    function value(index) {
        if (index < numVals)
            return repeater.itemAt(index).value;

        return 0;
    }

    Text {
        id: text
        text: isMin ? "Min" : "Max"
        anchors.verticalCenter: parent.verticalCenter
    }

    Repeater {
        id: repeater
        SpinBox {
            anchors.verticalCenter: text.verticalCenter
            width: 64 * scaleFactor
            minimumValue: 0
            maximumValue: maxRange
            value: isMin ? minimumValue : maximumValue
        }
    }
}
