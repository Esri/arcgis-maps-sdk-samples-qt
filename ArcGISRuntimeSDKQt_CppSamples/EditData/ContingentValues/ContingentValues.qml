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

        background: Rectangle {
            color: "#fdfdfd"
        }

        visible: contingentValuesSample.featureAttributesPaneVisibe

        contentItem: Column {
            id: column
            spacing: 5
            padding: 10

            Text {
                text: "Status"
                font.bold: true
                font.pointSize: 11
            }

            ComboBox {
                id: statusComboBox
                width: column.implicitWidth
                model: [""].concat(Object.keys(contingentValuesSample.statusValues));

                onCurrentValueChanged: {
                    if (statusComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }
                    contingentValuesSample.updateField("Status", statusComboBox.currentValue);
                    protectionComboBox.model = [""].concat(contingentValuesSample.getContingentValues("Protection", "ProtectionFieldGroup"));
                }
            }

            Text {
                text: "Protection"
                font.bold: true
                font.pointSize: 11
            }

            ComboBox {
                id: protectionComboBox
                model: [""];

                enabled: statusComboBox.currentText !== ""

                onCurrentValueChanged: {
                    if (protectionComboBox.currentValue === "")
                        return;

                    contingentValuesSample.updateField("Protection", protectionComboBox.currentValue);

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
                font.bold: true
                font.pointSize: 11
            }
            Text {
                text: rangeValuesSpinBox.from + " to " + rangeValuesSpinBox.to;
                anchors.horizontalCenter: parent.horizontalCenter
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
                    saveButton.enabled = contingentValuesSample.validateContingentValues();
                }
            }

            Button {
                id: saveButton
                Text {
                    text: "Save"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
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
                id: button
                Text {
                    text: "Discard"
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    contingentValuesSample.featureAttributesPaneVisibe = false
                }
            }
        }

        onVisibleChanged: {
            if (!visible)
                return;

            // Reset attribute panel values
            statusComboBox.currentIndex = 0;
            protectionComboBox.currentIndex = 0;
            rangeValuesSpinBox.from = 0;
            rangeValuesSpinBox.to = 0;
        }
    }
}
