// [WriteFile Name=ContingentValues, Category=EditData]
// [Legal]
// Copyright 2022 Esri.

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
import Esri.ArcGISRuntime 100.13
import Esri.ArcGISExtras 1.1


Rectangle {
    id: sampleWindow
    clip: true
    width: 800
    height: 600

    readonly property url dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"
    property int attributePromptX: 0
    property int attributePromptY: 0
    property var newFeature: null;

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Map {
            Basemap {
                ArcGISVectorTiledLayer {
                    url: dataPath + "vtpk/FillmoreTopographicMap.vtpk"
                    onErrorChanged: console.log("vtpk", error.message);
                }
            }

            ViewpointCenter {
                Point {
                    x: -13236000
                    y: 4081200
                    spatialReference: SpatialReference { wkid: 3857 }
                }
                targetScale: 8822
            }

            FeatureLayer {
                id: nestsLayer
                featureTable: gdb.geodatabaseFeatureTablesByTableName["BirdNests"] ?? null;

                Geodatabase {
                    id: gdb
                    path: dataPath + "geodatabase/ContingentValuesBirdNests.geodatabase"
                    onErrorChanged: console.log("gdb", error.message);

                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            gdb.geodatabaseFeatureTablesByTableName["BirdNests"].contingentValuesDefinition.load();
                        }
                    }


                }


                onLoadStatusChanged: {
                    console.log(nestsLayer.loadStatus);
                    if (nestsLayer.loadStatus === Enums.LoadStatusLoaded) {
                        bufferFeatures();
                    }
                }

                onErrorChanged: console.log("feature layer", error.message);
            }
            //! [FeatureLayer Geodatabase create]

            onErrorChanged: console.log("map", error.message);
        }

        GraphicsOverlay {
            id: bufferGraphcis
            renderer: SimpleRenderer {
                SimpleFillSymbol {
                    style: Enums.SimpleFillSymbolStyleForwardDiagonal
                    color: "red"
                    outline: SimpleLineSymbol {
                        style: Enums.SimpleLineSymbolStyleSolid
                        color: "black"
                        width: 2
                    }
                }
            }
        }

        onMouseClicked: {
            attributePromptX = mouse.x;
            attributePromptY = mouse.y;
            attributePrompt.visible = true;
            newFeature = nestsLayer.featureTable.createFeatureWithAttributes({}, mouse.mapPoint);
        }

        onErrorChanged: console.log("map view", error.message);
    }


    Control {
        id: attributePrompt

        // Expand the attributes pane depending on where the user clicks or taps
        x: (attributePromptX + width > sampleWindow.height ? attributePromptX - width : attributePromptX) ?? 0;
        y: (attributePromptY + height > sampleWindow.height ? attributePromptY - height : attributePromptY) ?? 0;

        background: Rectangle {
            color: "#fdfdfd"
        }

        visible: false

        contentItem: Column {
            id: inputColumn
            spacing: 5
            padding: 10

            Text {
                text: "Status"
                font.bold: true
                font.pointSize: 11
            }

            ComboBox {
                id: statusComboBox

                // This ComboBox is hardcoded, but can be dynamically obtained from the feature layer's domain values
                model: ["", "Occupied", "Unoccupied"]

                onCurrentValueChanged: {
                    console.log(newFeature);
                    if (statusComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }

                    // Update the feature's attribute map with the selection
                    newFeature.attributes["Status"] = statusComboBox.currentValue;
                    // Append the valid contingent coded values to the subsequent ComboBox
                    protectionComboBox.model = [""].concat(getContingentValues("Protection", "ProtectionFieldGroup"));
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

                    // Update the feature's attribute map with the selection
                    updateField("Protection", protectionComboBox.currentValue);

                    // Get the valid contingent range values for the subsequent SpinBox
                    const minMax = getContingentValues("BufferSize", "BufferSizeFieldGroup")

                    // If getContingentValues() returned results, update the spin box values
                    if (minMax[0] !== "") {
                        rangeValuesSpinBox.from = minMax[0];
                        rangeValuesSpinBox.to = minMax[1];

                        // If the maxValue in the range is 0, set the buffer size to 0
                        if (minMax[1] === 0) {
                            updateField("BufferSize", 0);
                            saveButton.enabled = validateContingentValues();
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

                    updateField("BufferSize", rangeValuesSpinBox.value);

                    // Validate that all contingencies are valid, if so, enable the save button
                    saveButton.enabled = validateContingentValues();
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
                    const valid = validateContingentValues();
                    if (valid) {
                        createNewNest();
                        attributePrompt.visible = false;
                    }
                }
            }

            Button {
                id: discardButton
                Text {
                    text: "Discard"
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    attributePrompt.visible = false
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

    function getContingentValues() {
        let contingentValuesResult = gdb.geodatabaseFeatureTablesByTableName["BirdNests"].contingentValues(newFeature, "Protection");
        const contingentValuesList = contingentValuesResult.contingentValuesByFieldGroup.ProtectionFieldGroup;

        console.log(contingentValuesList);
    }

    function validateContingentValues() {

    }

    function createNewNest() {

    }

    function bufferFeatures() {
        console.log("and here we should be buffering");
    }

    function updateField() {

    }
}
