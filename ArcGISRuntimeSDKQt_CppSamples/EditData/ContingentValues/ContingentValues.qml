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
        id: model
        mapView: view
    }

    Control {
        id: attributePrompt
        //        anchors {
        //            verticalCenter: sampleWindow.verticalCenter
        //            horizontalCenter: sampleWindow.horizontalCenter
        //        }
        x: model.featureAttributesPaneX + width > sampleWindow.height ? model.featureAttributesPaneX - width : model.featureAttributesPaneX
        y: model.featureAttributesPaneY + height > sampleWindow.height ? model.featureAttributesPaneY - height : model.featureAttributesPaneY

        height: 200
        width: 150

        background: Rectangle {
            color: "#d8e1ef"
        }

        contentItem: Column {
            Text { text: "Activity" }
            ComboBox {
                id: activityComboBox
                model: ["<NULL>", "Occupied", "Unoccupied"]
                currentIndex: ["OCCUPIED", "UNOCCUPIED"].indexOf(model.featureAttributes.Activity) + 1
                onCurrentIndexChanged: {
                    if (!model.featureAttributes.Activity)
                        return;

                    const currentActivity = ["OCCUPIED", "UNOCCUPIED"][currentIndex-1];

                    if (currentActivity === model.featureAttributes.Activity)
                        return;

                    const featureAttributeMap = model.featureAttributes
                    featureAttributeMap.Activity = currentActivity;
                    model.featureAttributes = featureAttributeMap;
                    let a = model.validateNestActivity(currentActivity);
                }
            }
            Text { text: "Protection Status" }
            ComboBox {
                id: protectionComboBox
                model: ["<NULL>", "Endangered", "Not Endangered"]
                currentIndex: ["ENDANGERED", "NOT_ENDANGERED"].indexOf(model.featureAttributes["Protection"]) + 1

                onCurrentIndexChanged: {
                    if (!model.featureAttributes.Activity)
                        return;

                    const currentActivity = ["OCCUPIED", "UNOCCUPIED"][currentIndex-1];

                    if (currentActivity === model.featureAttributes.Activity)
                        return;

                    const featureAttributeMap = model.featureAttributes
                    featureAttributeMap.Activity = currentActivity;
                    model.featureAttributes = featureAttributeMap;
                    let a = model.validateNestActivity(currentActivity);
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
                value: model.featureAttributes["BufferSize"] ?? 0;

                onValueChanged: {

                }


            }
            Row {
                Button {
                    Text {
                        text: "Save edits & close"
                    }
                    onClicked: {
                        model.featureAttributesPaneVisibe = false
                        console.log(activityComboBox.currentValue, protectionComboBox.currentValue, rangeValuesSpinBox.value);
                    }
                }
                Button {
                    Text {
                        text: "Discard edits & close"
                    }
                    onClicked: model.featureAttributesPaneVisibe = false
                }
                Button {
                    Text {
                        text: "Delete feature"
                    }
                    onClicked: {
                        model.featureAttributesPaneVisibe = false;
                        model.modifyFeatures([], "DELETE");
                    }
                }
            }
        }

        visible: model.featureAttributesPaneVisibe

        onVisibleChanged: {
            if (!visible)
                return;
        }
    }

    // Catch mouse signals so they don't propagate to the map
}
