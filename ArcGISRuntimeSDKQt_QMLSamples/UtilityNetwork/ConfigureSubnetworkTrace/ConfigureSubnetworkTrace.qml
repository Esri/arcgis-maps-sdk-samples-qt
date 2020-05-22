// [WriteFile Name=ConfigureSubnetworkTrace, Category=UtilityNetwork]
// [Legal]
// Copyright 2020 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12
import Esri.ArcGISRuntime 100.9

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var attributeStringListModel: []
    property var initialExpression: null
    property var networkDefinition: null
    property var traceConfiguration: null
    property var utilityElementStartingLocation: null
    readonly property string assetGroupName: "Circuit Breaker"
    readonly property string assetTypeName: "Three Phase"
    readonly property string deviceTableName: "Electric Distribution Device"
    readonly property string domainNetworkName: "ElectricDistribution"
    readonly property string globalId: "{1CAF7740-0BF4-4113-8DB2-654E18800028}"
    readonly property string tierName: "Medium Voltage Radial"
    readonly property url featureLayerUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"
    readonly property var attributeComparisonOperatorModel: ["Equal","NotEqual","GreaterThan","GreaterThanEqual","LessThan","LessThanEqual","IncludesTheValues","DoesNotIncludeTheValues","IncludesAny","DoesNotIncludeAny"]

    ListModel {
        id: valueSelectionListModel
    }

    UtilityNetwork {
        id: utilityNetwork
        url: featureLayerUrl

        onTraceStatusChanged: {
            if (traceStatus !== Enums.TaskStatusCompleted)
                return;

            if (traceResult.count === 0) {
                busy.visible = false;
                return;
            }

            // Get the first result.
            const myTraceResult = traceResult.get(0);
            const resultElements = myTraceResult.elements;

            // Display the number of elements found by the trace.
            dialogText.text = qsTr("%1 elements found.".arg(resultElements.length))
            dialog.open();
            busyIndicator.visible = false;
        }

        onErrorChanged: {
            dialogText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
            dialog.open();
            busyIndicator.visible = false;
        }

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            busyIndicator.visible = false;
            networkDefinition = utilityNetwork.definition;

            // Build the choice lists for network attribute comparison.
            for (let i = 0; i < definition.networkAttributes.length; i++) {
                if (!networkDefinition.networkAttributes[i].systemDefined)
                    attributeStringListModel.push(networkDefinition.networkAttributes[i].name);
            }

            networkAttributeComboBox.model = attributeStringListModel;

            const networkSource = networkDefinition.networkSource(deviceTableName);
            const assetGroup = networkSource.assetGroup(assetGroupName);
            const assetType = assetGroup.assetType(assetTypeName);

            // Create a default starting location.
            utilityElementStartingLocation = utilityNetwork.createElementWithAssetType(assetType, globalId);

            const terminals = utilityElementStartingLocation.assetType.terminalConfiguration.terminals;

            // Set the terminal for this location. (For our case, we use the 'Load' terminal.)
            for (let i = 0; i < terminals.length; i++) {
                const terminal = terminals[i];
                if (terminal.name === "Load")
                    utilityElementStartingLocation.terminal = terminal;
            }

            // Get a default trace configuration from a tier to update the UI.
            const domainNetwork = networkDefinition.domainNetwork(domainNetworkName);
            const utilityTierSource = domainNetwork.tier(tierName);

            // Set the trace configuration.
            traceConfiguration = utilityTierSource.traceConfiguration;

            initialExpression = traceConfiguration.traversability.barriers;

            // Set the traversability scope.
            traceConfiguration.traversability.scope = Enums.UtilityTraversabilityScopeJunctions;

            expressionBuilder.text = expressionToString(initialExpression);
        }

        onComponentCompleted: {
            busyIndicator.visible = true;
            load();
        }
    }

    function updateInputMethod(currentText, currentIndex) {
        if (networkDefinition) {
            const workingDomain = networkDefinition.networkAttribute(currentText).domain;
            if (workingDomain) {
                valueSelectionListModel.clear();
                for (let i = 0; i < workingDomain.codedValues.length; i++) {
                    valueSelectionListModel.append({value: workingDomain.codedValues[i].name})
                }
                valueSelectionComboBox.model = valueSelectionListModel;
                valueSelectionComboBox.currentIndex = 0;

                inputTextField.visible = false;
            } else {
                inputTextField.visible = true;
            }
        }
    }

    function expressionToString(expression) {
        const traceConditionType = expression.traceConditionType;
        const networkAttribute = expression.networkAttribute;
        const otherNetworkAttribute = expression.otherNetworkAttribute;
        const operatorAsString = comparisonOperatorToString(expression.comparisonOperator)

        switch (traceConditionType) {
        case Enums.UtilityTraceConditionTypeUtilityNetworkAttributeComparison:
            // check if attribute domain is a coded value domain.
            if (networkAttribute.domain && (networkAttribute.domain.domainType === Enums.DomainTypeCodedValueDomain)) {
                const dataType = networkAttribute.dataType;
                const domain = networkAttribute.domain;
                const codedValueName = domain.codedValues[expression.value].name;
                return "`%1` %2 `%3`".arg(networkAttribute.name).arg(operatorAsString).arg(codedValueName);
            } else {
                return "`%1` %2 `%3`".arg(networkAttribute.name).arg(operatorAsString).arg(otherNetworkAttribute? otherNetworkAttribute.name : expression.value)
            }
        case Enums.UtilityTraceConditionTypUtilityCategoryComparisone:
            return "`%1` %2".arg(networkAttribute.category.name).arg(networkAttribute.comparisonOperator === Enums.UtilityCategoryComparisonOperatorExists ? "Exists" : "DoesNotExist");
        case Enums.UtilityTraceConditionTypeUtilityTraceAndCondition:
            return "`%1` AND\n `%3`".arg(expressionToString(expression.leftExpression)).arg(expressionToString(expression.rightExpression));
        case Enums.UtilityTraceConditionTypeUtilityTraceOrCondition:
            return "`%1` OR\n `%3`".arg(expressionToString(expression.leftExpression)).arg(expressionToString(expression.rightExpression));
        default:
            return "Unknown trace conditional expression";
        }
    }

    function comparisonOperatorToString(comparisonOperator) {
        switch (comparisonOperator) {
        case Enums.UtilityAttributeComparisonOperatorEqual:
            return "Equal";
        case Enums.UtilityAttributeComparisonOperatorNotEqual:
            return "NotEqual";
        case Enums.UtilityAttributeComparisonOperatorGreaterThan:
            return "GreaterThan";
        case Enums.UtilityAttributeComparisonOperatorGreaterThanEqual:
            return "GreaterThanEqual";
        case Enums.UtilityAttributeComparisonOperatorLessThan:
            return "LessThan";
        case Enums.UtilityAttributeComparisonOperatorLessThanEqual:
            return "LessThanEqual";
        case Enums.UtilityAttributeComparisonOperatorIncludesTheValues:
            return "IncludesTheValues";
        case Enums.UtilityAttributeComparisonOperatorDoesNotIncludeTheValues:
            return "DoesNotIncludeTheValues";
        case Enums.UtilityAttributeComparisonOperatorIncludesAny:
            return "IncludesAny";
        case Enums.UtilityAttributeComparisonOperatorDoesNotIncludeAny:
            return "DoesNotIncludeAny";
        default:
            return "Unknown comparison operator";
        }
    }

    function addCondition() {
        // NOTE: You may also create a UtilityCategoryComparison with UtilityNetworkDefinition.Categories and UtilityCategoryComparisonOperator.

        const selectedNetworkAttribute = networkDefinition.networkAttribute(networkAttributeComboBox.currentText);
        const data = convertToDataType(valueSelectionComboBox.visible ? valueSelectionComboBox.currentIndex : inputTextField.text , selectedNetworkAttribute.dataType)

        // check to make sure the conversion was successful.
        if (!data) {
            dialogText.text = qsTr("Invalid input. Please try again.")
            dialog.open();
            return;
        }

        // NOTE: You may also create a UtilityNetworkAttributeComparison with another NetworkAttribute.
        const expression = ArcGISRuntimeEnvironment.createObject("UtilityNetworkAttributeComparison", {
                                                                     networkAttribute: selectedNetworkAttribute,
                                                                     comparisonOperator: comparisonOperatorComboBox.currentIndex,
                                                                     value: data});

        const otherExpression = traceConfiguration.traversability.barriers;

        // NOTE: You may also combine expressions with UtilityTraceAndCondition
        const combineExpressions = ArcGISRuntimeEnvironment.createObject("UtilityTraceOrCondition", {
                                                                             leftExpression: otherExpression,
                                                                             rightExpression: expression
                                                                         });

        traceConfiguration.traversability.barriers = combineExpressions;
        expressionBuilder.text = expressionToString(combineExpressions);
    }

    function convertToDataType(data, dataType) {
        if (!data)
            return;
        switch(dataType) {
        case Enums.UtilityNetworkAttributeDataTypeInteger:
            return parseInt(data, 10);
        case Enums.UtilityNetworkAttributeDataTypeFloat:
        case Enums.UtilityNetworkAttributeDataTypeDouble:
            return data;
        case Enums.UtilityNetworkAttributeDataTypeBoolean:
            return data == 1 ? true : false;
        default:
            return;
        }
    }

    function reset() {
        traceConfiguration.traversability.barriers = initialExpression;
        expressionBuilder.text = expressionToString(initialExpression);
    }

    function trace() {
        if (!utilityNetwork || !utilityElementStartingLocation) {
            return;
        }

        busyIndicator.visible = true;
        const startingLocations = [utilityElementStartingLocation];

        // initialize utility trace parameters with the starting location.
        params.startingLocations = startingLocations;
        params.traceType = Enums.UtilityTraceTypeSubnetwork;
        params.traceConfiguration = traceConfiguration;

        // trace the network
        utilityNetwork.trace(params);
    }

    UtilityTraceParameters {
        id: params
    }

    Rectangle {
        anchors.fill: parent
        color: "lightgrey"

        ScrollView {
            id: scrollView
            anchors.fill: parent

            ColumnLayout {
                id: controlItemsLayout
                anchors.left: parent.left
                anchors.right: parent.right

                CheckBox {
                    text: qsTr("Include barriers")
                    Layout.fillWidth: true
                    checkState: Qt.Checked
                    enabled: !busyIndicator.visible
                    onCheckStateChanged: traceConfiguration.includeBarriers = checked;
                }

                CheckBox {
                    text: qsTr("Include containers")
                    Layout.fillWidth: true
                    checkState: Qt.Checked
                    enabled: !busyIndicator.visible
                    onCheckStateChanged: traceConfiguration.includeContainers = checked;
                }

                Shape {
                    height: 2
                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"
                        strokeStyle: ShapePath.SolidLine
                        startX: 2; startY: 0
                        PathLine { x: controlItemsLayout.width - 2 ; y: 0 }
                    }
                }

                Text {
                    text: qsTr("Example barrier condition for this data. 'Transformer Load' Equal '15'")
                    font.pixelSize: 11
                    Layout.minimumWidth: rootRectangle.width
                    horizontalAlignment: Text.AlignHCenter
                }

                Shape {
                    height: 2
                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"
                        strokeStyle: ShapePath.SolidLine
                        startX: 2; startY: 0
                        PathLine { x: controlItemsLayout.width - 2 ; y: 0 }
                    }
                }

                Text {
                    text: qsTr("New Barrier Condition:")
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: networkAttributeComboBox
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                    onCurrentTextChanged: updateInputMethod(currentText, currentIndex);
                }

                ComboBox {
                    id: comparisonOperatorComboBox
                    model: attributeComparisonOperatorModel
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                }

                RowLayout {
                    ComboBox {
                        id: valueSelectionComboBox
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        visible: !inputTextField.visible
                    }

                    TextField {
                        id: inputTextField
                        Layout.minimumHeight: valueSelectionComboBox.height
                        Layout.fillWidth: true
                        visible: true
                        validator: DoubleValidator {}
                        color: "black"
                        placeholderText: qsTr("Enter value here")
                        placeholderTextColor: "black"
                        selectByMouse: true
                        background: Rectangle {
                            anchors.centerIn: parent
                            height: parent.height
                            width: parent.width
                            color: "white"
                        }
                    }
                }

                Button {
                    text: qsTr("Add")
                    Layout.fillWidth: true
                    enabled: !busyIndicator.visible
                    onClicked: addCondition();
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.maximumWidth: rootRectangle.width
                    Layout.minimumHeight: 50
                    Layout.maximumHeight: .15 * rootRectangle.height
                    clip: true
                    Row {
                        anchors.fill: parent
                        Text {
                            id: expressionBuilder
                        }
                    }
                }

                RowLayout {
                    Button {
                        text: qsTr("Trace")
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        onClicked: trace();
                    }

                    Button {
                        text: qsTr("Reset")
                        Layout.fillWidth: true
                        enabled: !busyIndicator.visible
                        onClicked: reset();
                    }
                }
            }
        }
    }

    Dialog {
        id: dialog
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Text {
            id: dialogText
            anchors.centerIn: parent
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
    }
}
