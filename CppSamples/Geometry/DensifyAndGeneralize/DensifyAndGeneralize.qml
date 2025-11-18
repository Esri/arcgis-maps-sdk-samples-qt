// [WriteFile Name=DensifyAndGeneralize, Category=Geometry]
// [Legal]
// Copyright 2018 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import Esri.Samples

DensifyAndGeneralizeSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        width: controlColumn.width + 16
        height: controlColumn.height + 16
        anchors{
            top: parent.top
            right: parent.right
        }

        color: palette.base
        radius: 5
        border {
            color: "darkgrey"
            width: 1
        }

        Column {
            id: controlColumn
            anchors {
                centerIn: parent
                margins: 10
            }
            spacing: 10


            CheckBox {
                id: densifyCheckbox
                text: qsTr("Densify")
                checked: true
            }

            Label {
                text: qsTr("Max segment length")
                enabled: densifyCheckbox.checked
            }

            Slider {
                id: maxSegmentLengthSlider
                from: 100
                to: 500
                width: 175
                value: 100
                onValueChanged: updateGeometry(densifyCheckbox.checked, maxSegmentLengthSlider.value, generalizeCheckbox.checked, maxDeviationSlider.value);
            }

            CheckBox {
                id: generalizeCheckbox
                text: qsTr("Generalize")
                checked: true
            }

            Label {
                text: qsTr("Max deviation")
                enabled: generalizeCheckbox.checked
            }

            Slider {
                id: maxDeviationSlider
                from: 1
                to: 250
                width: 175
                onValueChanged: updateGeometry(densifyCheckbox.checked, maxSegmentLengthSlider.value, generalizeCheckbox.checked, maxDeviationSlider.value);
            }

            CheckBox {
                id: showResultCheckbox
                text: qsTr("Show Result")
                checked: true
                onCheckedChanged: showResults(checked);
            }
        }
    }
}
