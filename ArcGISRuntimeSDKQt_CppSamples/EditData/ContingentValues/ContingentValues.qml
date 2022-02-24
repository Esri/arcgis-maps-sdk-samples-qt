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

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ContingentValuesSample {
        id: contingentValuesSample
        mapView: view
    }

    Control {
        id: attributePrompt
        padding: 5

        // Expand the attributes pane depending on where the user clicks or taps
        x: sampleWindow.width - (attributePrompt.width + attributePrompt.padding)
        y: attributePrompt.padding

        background: Rectangle {
            color: "#fdfdfd"
        }

        visible: contingentValuesSample.featureAttributesPaneVisibe

        contentItem: Column {
            id: inputColumn
            spacing: 5
            padding: 10

            Text {
                text: "Status"
                font {
                    bold: true
                    pointSize: 11
                }
            }

            ComboBox {
                id: statusComboBox

                // This ComboBox is hardcoded, but can be dynamically obtained from the feature layer's domain values
                model: [""].concat(Object.keys(contingentValuesSample.statusValues));

                onCurrentValueChanged: {
                    if (statusComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }

                    // Update the feature's attribute map with the selection
                    contingentValuesSample.updateField("Status", statusComboBox.currentValue);
                    // Append the valid contingent coded values to the subsequent ComboBox
                    protectionComboBox.model = [""].concat(contingentValuesSample.getContingentValues("Protection", "ProtectionFieldGroup"));
                }
            }

            Text {
                text: "Protection"
                font {
                    bold: true
                    pointSize: 11
                }
            }

            ComboBox {
                id: protectionComboBox
                model: [""];

                enabled: statusComboBox.currentText !== ""

                onCurrentValueChanged: {
                    if (protectionComboBox.currentValue === "")
                        return;

                    // Update the feature's attribute map with the selection
                    contingentValuesSample.updateField("Protection", protectionComboBox.currentValue);

                    // Get the valid contingent range values for the subsequent SpinBox
                    const minMax = contingentValuesSample.getContingentValues("BufferSize", "BufferSizeFieldGroup")

                    // If getContingentValues() returned results, update the spin box values
                    if (minMax[0] !== "") {
                        rangeValuesSpinBox.from = minMax[0];
                        rangeValuesSpinBox.to = minMax[1];

                        // If the maxValue in the range is 0, set the buffer size to 0
                        if (minMax[1] === 0) {
                            contingentValuesSample.updateField("BufferSize", 0);
                            saveButton.enabled = contingentValuesSample.validateContingentValues();
                        }
                    }
                }
            }

            Text {
                text: "Buffer Size"
                font {
                    bold: true
                    pointSize: 11
                }
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
                value: 0;

                enabled: protectionComboBox.currentText !== ""

                onValueChanged: {
                    if (protectionComboBox.currentValue === "")
                        return;

                    contingentValuesSample.updateField("BufferSize", rangeValuesSpinBox.value);

                    // Validate that all contingencies are valid, if so, enable the save button
                    saveButton.enabled = contingentValuesSample.validateContingentValues();
                }
            }

            Button {
                id: saveButton
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: "Save"
                }

                enabled: false

                onClicked: {
                    const valid = contingentValuesSample.validateContingentValues();
                    if (valid) {
                        contingentValuesSample.createNewNest();
                        contingentValuesSample.featureAttributesPaneVisibe = false;
                    }
                }
            }

            Button {
                id: discardButton
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: "Discard"
                }
                onClicked: {
                    contingentValuesSample.featureAttributesPaneVisibe = false;
                    contingentValuesSample.discardFeature();
                }
            }
        }

        onVisibleChanged: {
            if (!visible)
                return;

            // Reset attribute panel values when the panel opens
            statusComboBox.currentIndex = 0;
            protectionComboBox.currentIndex = 0;
            rangeValuesSpinBox.from = 0;
            rangeValuesSpinBox.to = 0;
        }
    }
}
