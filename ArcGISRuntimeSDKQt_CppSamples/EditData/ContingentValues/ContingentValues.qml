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

        x: contingentValuesSample.featureAttributesPaneX + width > sampleWindow.height ? contingentValuesSample.featureAttributesPaneX - width : contingentValuesSample.featureAttributesPaneX
        y: contingentValuesSample.featureAttributesPaneY + height > sampleWindow.height ? contingentValuesSample.featureAttributesPaneY - height : contingentValuesSample.featureAttributesPaneY

        height: 200
        width: 150

        background: Rectangle {
            color: "#d8e1ef"
        }

        contentItem: Column {
            Text { text: "Status" }
            ComboBox {
                id: activityComboBox
                model: [""].concat(contingentValuesSample.activityValues) ?? [""];

                enabled: true

                onCurrentValueChanged: {
                    if (activityComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }

                    const featureAttributeMap = contingentValuesSample.featureAttributes
                    contingentValuesSample.updateField("Status", activityComboBox.currentValue);

                    console.log(contingentValuesSample.featureAttributes);

                    contingentValuesSample.protectionValues = contingentValuesSample.getContingentValues("Protection", "ProtectionFieldGroup");
                    protectionComboBox.model = [""].concat(contingentValuesSample.protectionValues) ?? [""];

                }
            }
            Text { text: "Protection Status" }
            ComboBox {
                id: protectionComboBox
                model: [""].concat(contingentValuesSample.protectionValues) ?? [""];

                enabled: activityComboBox.currentText !== ""

                onCurrentValueChanged: {
                    //return;
                    if (protectionComboBox.currentValue === "")
                        return;

                    console.log("hello");
                    const featureAttributeMap = contingentValuesSample.featureAttributes
                    featureAttributeMap.Protection = protectionComboBox.currentValue;
                    contingentValuesSample.featureAttributes = featureAttributeMap;

                    const minMax = contingentValuesSample.getContingentValues("BufferSize", "BufferSizeFieldGroup")
                    console.log("minMax is", minMax);
                    if (minMax[0] !== "") {
                    rangeValuesSpinBox.from = minMax[0];
                    rangeValuesSpinBox.to = minMax[1];
                    } else {
                        rangeValuesSpinBox.from = 0;
                        rangeValuesSpinBox.to = 0;
                    }


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
                from: contingentValuesSample.bufferSizeValues[0] ?? 0;
                to: contingentValuesSample.bufferSizeValues[1] ?? 120;
                stepSize: 10
                value: contingentValuesSample.featureAttributes["BufferSize"] ?? 0;

                enabled: protectionComboBox.currentText !== ""

                onValueChanged: {
                    let temp = contingentValuesSample.featureAttributes;
                    temp.BufferSize = rangeValuesSpinBox.value;
                    contingentValuesSample.featureAttributes = temp;
                }


            }
            Button {
                Text {
                    text: "Save edits & close"
                }
                onClicked: {
                    contingentValuesSample.modifyFeatures([], "SAVE");
                    contingentValuesSample.featureAttributesPaneVisibe = false;
                }
            }
            Button {
                Text {
                    text: "Discard edits & close"
                }
                onClicked: {
                 contingentValuesSample.modifyFeatures([], "CANCEL");
                    contingentValuesSample.featureAttributesPaneVisibe = false
                }
            }
            Button {
                Text {
                    text: "Delete feature"
                }
                onClicked: {
                    contingentValuesSample.modifyFeatures([], "DELETE");
                    contingentValuesSample.featureAttributesPaneVisibe = false;
                }
            }
        }

        visible: contingentValuesSample.featureAttributesPaneVisibe

        onVisibleChanged: {
            if (!visible)
                return;
        }
    }
}
