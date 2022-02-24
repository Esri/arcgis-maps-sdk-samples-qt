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

    // Feature to define attributes for with ContingentValues
    property var newFeature: null;

    // TaskId to track the queryFeatures() function of the GeodatabaseFeatureTable
    property var taskId;

    // GeodatabaseFeatureTable
    property var birdNestsTable: null;

    // Field Domains are hardcoded to simplify this sample, but can be obtained from a FeatureTable's Domain
    property var statusValues: {"Occupied":"OCCUPIED", "Unoccupied":"UNOCCUPIED"};
    property var protectionValues: {"Endangered":"ENDANGERED", "Not endangered":"NOT_ENDANGERED", "N/A":"NA"};

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
                featureTable: birdNestsTable ?? null;

                Geodatabase {
                    id: gdb
                    path: dataPath + "geodatabase/ContingentValuesBirdNests.geodatabase"

                    onLoadStatusChanged: {
                        if (loadStatus === Enums.LoadStatusLoaded) {
                            birdNestsTable = gdb.geodatabaseFeatureTablesByTableName["BirdNests"];
                            birdNestsTable.load();
                        }
                    }
                }

                onLoadStatusChanged: {
                    if (nestsLayer.loadStatus === Enums.LoadStatusLoaded) {
                        // Load the ContingentValuesDefinition to enable access to the GeodatabaseFeatureTable's ContingentValues data such as FieldGroups and ContingentValuesResults
                        birdNestsTable.contingentValuesDefinition.load();

                        // Load map with initial nest buffers showing
                        bufferFeatures();
                    }
                }
            }
        }

        GraphicsOverlay {
            id: bufferGraphicsOverlay
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
            attributePrompt.visible = true;

            // Create a new feature to append attribute values to
            newFeature = birdNestsTable.createFeatureWithAttributes({}, mouse.mapPoint);
            birdNestsTable.addFeature(newFeature);
        }
    }


    Control {
        id: attributePrompt
        anchors {
            top: sampleWindow.top
            right: sampleWindow.right
            margins: 5
        }

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
                font {
                    bold: true
                    pointSize: 11
                }
            }

            ComboBox {
                id: statusComboBox

                // This ComboBox is hardcoded, but can be dynamically obtained from the feature layer's field's domain values
                model: ["", "Occupied", "Unoccupied"]

                onCurrentValueChanged: {
                    if (statusComboBox.currentValue === "") {
                        protectionComboBox.model = [""];
                        return;
                    }

                    // Update the feature's attribute map with the selection
                    newFeature.attributes.replaceAttribute("Status", statusValues[statusComboBox.currentValue]);

                    // Append the valid contingent coded values to the subsequent ComboBox
                    protectionComboBox.model = [""].concat(getContingentValues("Protection", "ProtectionFieldGroup"));
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
                    newFeature.attributes.replaceAttribute("Protection", protectionValues[protectionComboBox.currentValue]);

                    // Get the valid contingent range values for the subsequent SpinBox
                    const minMax = getContingentValues("BufferSize", "BufferSizeFieldGroup")

                    // If getContingentValues() returned results, update the spin box values
                    if (minMax[0] !== "") {
                        rangeValuesSpinBox.from = minMax[0];
                        rangeValuesSpinBox.to = minMax[1];

                        // Explicitly set the buffer size to the rangeValuesSpinBox value in case onValueChanged doesn't trigger
                        newFeature.attributes.replaceAttribute("BufferSize", rangeValuesSpinBox.value);
                        saveButton.enabled = validateContingentValues();
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

                    newFeature.attributes.replaceAttribute("BufferSize", rangeValuesSpinBox.value);

                    // Validate that all contingencies are valid, if so, enable the save button
                    saveButton.enabled = validateContingentValues();
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
                    const valid = validateContingentValues();
                    if (validateContingentValues()) {
                        birdNestsTable.updateFeature(newFeature);
                        bufferFeatures();
                        attributePrompt.visible = false;
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
                    birdNestsTable.deleteFeature(newFeature);
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

    function getContingentValues(field, fieldGroup) {
        const contingentValuesResult = birdNestsTable.contingentValues(newFeature, field);
        const contingentValuesList = contingentValuesResult.contingentValuesByFieldGroup[fieldGroup];

        const returnList = [];

        contingentValuesList.forEach(contingentValue => {
                                         if (contingentValue.objectType === Enums.ContingentValueTypeContingentCodedValue) {
                                             returnList.push(contingentValue.codedValue.name);
                                         } else if (contingentValue.objectType === Enums.ContingentValueTypeContingentRangeValue) {
                                             returnList.push(contingentValue.minValue);
                                             returnList.push(contingentValue.maxValue);
                                         }
                                     });
        return returnList;
    }

    function validateContingentValues() {
        const contingencyConstraintsList = birdNestsTable.validateContingencyConstraints(newFeature);

        if (contingencyConstraintsList.length === 0)
            return true;

        return false;
    }

    QueryParameters {
        id: bufferParameters
        whereClause: "BufferSize > 0"
    }

    // This function creates a buffer around nest features based on their BufferSize value
    // It is included to demonstrate the sample use case
    function bufferFeatures() {
        const featureStatusChanged = ()=> {
            switch (birdNestsTable.queryFeaturesStatus) {
                case Enums.TaskStatusCompleted:
                birdNestsTable.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                bufferGraphicsOverlay.graphics.clear();

                const result = birdNestsTable.queryFeaturesResults[taskId];
                if (result) {
                    const features = Array.from(result.iterator.features);
                    features.forEach(feature => {
                                         const buffer = GeometryEngine.buffer(feature.geometry, feature.attributes.attributeValue("BufferSize"));
                                         const bufferGraphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: buffer});
                                         bufferGraphicsOverlay.graphics.append(bufferGraphic);
                                     });
                } else {
                    console.log("The query finished but there was no result for this taskId");
                }
                break;

                case Enums.TaskStatusErrored:
                birdNestsTable.queryFeaturesStatusChanged.disconnect(featureStatusChanged);
                if (birdNestsTable.error) {
                    console.log("Feature query error", birdNestsTable.error.message, birdNestsTable.error.additionalMessage);
                } else {
                    console.log("The query encountered an error, but provided no error message");
                }
                break;

                default:
                break;
            }
        }

        birdNestsTable.queryFeaturesStatusChanged.connect(featureStatusChanged);
        taskId = birdNestsTable.queryFeatures(bufferParameters);
    }
}
