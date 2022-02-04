// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2021 Esri.

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

import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {
    id: sampleWindow

    // add a mapView component
    MapView {
        id: view
        anchors.fill: sampleWindow

        MouseArea {
            anchors.fill: view
            visible: attributePrompt.visible
            onClicked: mouse.accepted = attributePrompt.visible
            onWheel: wheel.accepted = attributePrompt.visible
        }

        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ContingentValuesSample {
        id: contingentValuesSample
        mapView: view
    }

    Control {
        id: attributePrompt

        x: (contingentValuesSample.featureAttributesPaneXY[0] + width > sampleWindow.height ? contingentValuesSample.featureAttributesPaneXY[0] - width : contingentValuesSample.featureAttributesPaneXY[0]) ?? 0;
        y: (contingentValuesSample.featureAttributesPaneXY[1] + height > sampleWindow.height ? contingentValuesSample.featureAttributesPaneXY[1] - height : contingentValuesSample.featureAttributesPaneXY[1]) ?? 0;
        height: 200
        width: 150

        background: Rectangle {
            color: "#3168ba" //"#d8e1ef"
        }

        contentItem: Column {
            Text { text: "Status" }
            ComboBox {
                id: activityComboBox
                model: [""].concat(Object.keys(contingentValuesSample.activityValues));

                enabled: true

                onCurrentValueChanged: {
                    if (activityComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }
                    contingentValuesSample.updateField("Status", activityComboBox.currentValue);
                    protectionComboBox.model = [""].concat(contingentValuesSample.getContingentValues("Protection", "ProtectionFieldGroup"));
                }
            }
            Text { text: "Protection Status" }
            ComboBox {
                id: protectionComboBox
                model: [""];

                enabled: activityComboBox.currentText !== ""

                onCurrentValueChanged: {
                    if (protectionComboBox.currentValue === "")
                        return;

                    contingentValuesSample.updateField("Protection", protectionComboBox.currentValue);

                    const minMax = contingentValuesSample.getContingentValues("BufferSize", "BufferSizeFieldGroup")
                    if (minMax[0] !== "") {
                        rangeValuesSpinBox.from = minMax[0];
                        rangeValuesSpinBox.to = minMax[1];
                    }
                    if (minMax[1] === 0)
                        contingentValuesSample.updateField("BufferSize", 0);
                }
            }

            Text {
                text: "Buffer Size"
            }
            Text {
                text: rangeValuesSpinBox.from + " to " + rangeValuesSpinBox.to;
            }
            SpinBox {
                id: rangeValuesSpinBox
                editable: true
                from: 0;
                to: 0;
                stepSize: 10
                value: contingentValuesSample.featureAttributes["BufferSize"] ?? 0;

                enabled: protectionComboBox.currentText !== ""

                onValueChanged: {
                    if (protectionComboBox.currentValue === "")
                        return;

                    contingentValuesSample.updateField("BufferSize", rangeValuesSpinBox.value);
                }
            }
            Button {
                Text {
                    text: "Save"
                }
                onClicked: {
                    const valid = contingentValuesSample.validateContingentValues();
                    if (valid) {
                        contingentValuesSample.createNewNest();
                        contingentValuesSample.featureAttributesPaneVisibe = false;
                    } else {
                        console.log("feature not valid");
                    }
                }
            }
            Button {
                Text {
                    text: "Discard"
                }
                onClicked: {
                    contingentValuesSample.featureAttributesPaneVisibe = false
                }
            }
        }

        visible: contingentValuesSample.featureAttributesPaneVisibe

        onVisibleChanged: {
            if (!visible)
                return;

            activityComboBox.currentIndex = 0;
            protectionComboBox.currentIndex = 0;
            rangeValuesSpinBox.from = 0;
            rangeValuesSpinBox.to = 0;
        }
    }
}
