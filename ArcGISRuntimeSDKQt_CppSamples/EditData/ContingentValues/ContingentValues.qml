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

    property bool init: false

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
        //        anchors {
        //            verticalCenter: sampleWindow.verticalCenter
        //            horizontalCenter: sampleWindow.horizontalCenter
        //        }
        x: contingentValuesSample.featureAttributesPaneX + width > sampleWindow.height ? contingentValuesSample.featureAttributesPaneX - width : contingentValuesSample.featureAttributesPaneX
        y: contingentValuesSample.featureAttributesPaneY + height > sampleWindow.height ? contingentValuesSample.featureAttributesPaneY - height : contingentValuesSample.featureAttributesPaneY

        height: 200
        width: 150

        background: Rectangle {
            color: "#d8e1ef"
        }

        contentItem: Column {
            Text { text: "Activity" }
            ComboBox {
                id: activityComboBox
                model: ["Occupied", "Unoccupied"]
                currentIndex: ["OCCUPIED", "UNOCCUPIED"].indexOf(contingentValuesSample.featureAttributes.Activity)
                onCurrentIndexChanged: {
                    const currentActivity = ["OCCUPIED", "UNOCCUPIED"][currentIndex];

                    if (currentActivity === contingentValuesSample.featureAttributes.Activity)
                        return;

                    const featureAttributeMap = contingentValuesSample.featureAttributes
                    featureAttributeMap.Activity = currentActivity;
                    contingentValuesSample.featureAttributes = featureAttributeMap;

                    console.log("running get contingent values with activity", currentActivity);

                    init = true;

                    protectionComboBox.model = contingentValuesSample.getContingentValues("Protection", "ProtectionFieldGroup");
                }
            }
            Text { text: "Protection Status" }
            ComboBox {
                id: protectionComboBox
                model: [""]
                currentIndex: ["", "ENDANGERED", "NOT_ENDANGERED"].indexOf(contingentValuesSample.featureAttributes.Protection)

                onCurrentIndexChanged: {
                    if (!contingentValuesSample.featureAttributes.Activity)
                        return;

                    if (init) {
                        init = false;
                        return;
                    }

                    const currentStatus = ["","ENDANGERED", "NOT_ENDANGERED"][currentIndex];

                    if (currentStatus === contingentValuesSample.featureAttributes.Protection)
                        return;

                    const featureAttributeMap = contingentValuesSample.featureAttributes
                    featureAttributeMap.Protection = currentStatus;
                    contingentValuesSample.featureAttributes = featureAttributeMap;

                    const minMax = contingentValuesSample.getContingentValues("BufferSize", "BufferSizeFieldGroup")
                    console.log(minMax);
                    //                    rangeValuesSpinBox.from = minMax[0];
                    //                    rangeValuesSpinBox.to = minMax[1];
                }
            }

            Text {
                text: "Buffer Size"
            }
            SpinBox {
                id: rangeValuesSpinBox
                editable: true
                from: 0
                to: 100
                stepSize: 10
                value: contingentValuesSample.featureAttributes["BufferSize"] ?? 0;

                onValueChanged: {

                }


            }
            Button {
                Text {
                    text: "Save edits & close"
                }
                onClicked: {
                    contingentValuesSample.featureAttributesPaneVisibe = false
                    console.log(activityComboBox.currentValue, protectionComboBox.currentValue, rangeValuesSpinBox.value);
                }
            }
            Button {
                Text {
                    text: "Discard edits & close"
                }
                onClicked: contingentValuesSample.featureAttributesPaneVisibe = false
            }
            Button {
                Text {
                    text: "Delete feature"
                }
                onClicked: {
                    contingentValuesSample.featureAttributesPaneVisibe = false;
                    contingentValuesSample.modifyFeatures([], "DELETE");
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
