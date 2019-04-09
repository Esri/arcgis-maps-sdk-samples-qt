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
    anchors.horizontalCenter: parent.horizontalCenter

    property alias maxRange : slider.to
    property alias value: slider.value
    property alias label: text.text

    Text {
        id: text
    }

    Slider {
        id: slider
        width: 100
        from: 0
        to: maxRange

        onValueChanged: {
            if (spinBox.value !== value)
                spinBox.value = value;
        }
    }

    SpinBox {
        id: spinBox
        from: 0
        to: slider.to

        onValueChanged: {
            if (slider.value !== value)
                slider.value = value;
        }
    }
}
