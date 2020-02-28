// [WriteFile Name=ConfigureSubnetworkTrace, Category=Analysis]
// [Legal]
// Copyright 2019 Esri.

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
import Esri.ArcGISRuntime 100.8

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var attributeStringListModel: []
    property var initialExpression
    property var networkDefinition
    property var traceConfiguration
    property var utilityElementStartingLocation
    readonly property string assetGroupName: "Circuit Breaker"
    readonly property string assetTypeName: "Three Phase"
    readonly property string deviceTableName: "Electric Distribution Device"
    readonly property string domainNetworkName: "ElectricDistribution"
    readonly property string globalId: "{1CAF7740-0BF4-4113-8DB2-654E18800028}"
    readonly property string tierName: "Medium Voltage Radial"
    readonly property url featureLayerUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"
    readonly property var comparisonOperatorModel: ["Equal","NotEqual","GreaterThan","GreaterThanEqual","LessThan","LessThanEqual","IncludesTheValues","DoesNotIncludeTheValues","IncludesAny","DoesNotIncludeAny"]

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

            const myTraceResult = traceResult.get(0);
            const resultElements = myTraceResult.elements;

            dialogText.text = qsTr("%1 elements found.".arg(resultElements.length))
            dialog.visible = true;
            busyIndicator.visible = false;
        }

        onErrorChanged: {
            dialogText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
            dialog.visible = true;
        }

        onLoadStatusChanged: {
            if(loadStatus !== Enums.LoadStatusLoaded)
                return;

            networkDefinition = utilityNetwork.definition;

            for (let i = 0; i < definition.networkAttributes.length; i++) {
                if (!networkDefinition.networkAttributes[i].systemDefined)
                    attributeStringListModel.push(networkDefinition.networkAttributes[i].name);
            }

            networkAttributeComboBox.model = attributeStringListModel;

            const networkSource = networkDefinition.networkSource(deviceTableName);
            const assetGroup = networkSource.assetGroup(assetGroupName);
            const assetType = assetGroup.assetType(assetTypeName);

            utilityElementStartingLocation = utilityNetwork.createElementWithAssetType(assetType, globalId);

            const terminals = utilityElementStartingLocation.assetType.terminalConfiguration.terminals;

            for (let i = 0; i < terminals.length; i++) {
                let terminal = terminals[i];
                if (terminal.name === "Load")
                    utilityElementStartingLocation.terminal = terminal;
            }

            const domainNetwork = networkDefinition.domainNetwork(domainNetworkName);
            const utilityTierSource = domainNetwork.tier(tierName);

            traceConfiguration = utilityTierSource.traceConfiguration;

            initialExpression = traceConfiguration.traversability.barriers;

            traceConfiguration.traversability.scope = Enums.UtilityTraversabilityScopeJunctions;

            expressionBuilder.text = expressionToString(initialExpression);
        }

        onComponentCompleted: load();
    }

    function updateInputMethod(currentText, currentIndex) {
        if(networkDefinition) {
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

        switch (traceConditionType) {
        case Enums.UtilityTraceConditionTypeUtilityNetworkAttributeComparison:
            if (networkAttribute.domain && (networkAttribute.domain.domainType === Enums.DomainTypeCodedValueDomain)) {
                const dataType = networkAttribute.dataType;
                const domain = networkAttribute.domain;
                return "`%1` %2 `%3`".arg(networkAttribute.name).arg(comparisonOperatorToString(expression.comparisonOperator)).arg(domain.codedValues[expression.value].name);
            } else {
                return "`%1` %2 `%3`".arg(networkAttribute.name).arg(comparisonOperatorToString(expression.comparisonOperator)).arg(otherNetworkAttribute? otherNetworkAttribute.name : expression.value)
            }
        case Enums.UtilityTraceConditionTypUtilityCategoryComparisone:
            return "`%1` %2".arg(networkAttribute.category.name).arg(networkAttribute.comparisonOperator === Enums.UtilityCategoryComparisonOperatorExists ? "Exists" : "DoesNotExist");
        case Enums.UtilityTraceConditionTypeUtilityTraceAndCondition:
            return "`%1` AND\n `%3`".arg(expressionToString(expression.leftExpression)).arg(expressionToString(expression.rightExpression));
        case Enums.UtilityTraceConditionTypeUtilityTraceOrCondition:
            return "`%1` OR\n `%3`".arg(expressionToString(expression.leftExpression)).arg(expressionToString(expression.rightExpression));
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
            return null;
        }
    }

    function addCondition() {
        if (traceConfiguration === null) {
            traceConfiguration = ArcGISRuntimeEnvironment.createObject("UtilityTraceConfiguration");
        }
        if (traceConfiguration.traversability === null) {
            traceConfiguration.traversability = ArcGISRuntimeEnvironment.createObject("UtilityTraversability");
        }

        const selectedNetworkAttribute = networkDefinition.networkAttribute(networkAttributeComboBox.currentText);
        const data = convertToDataType(valueSelectionComboBox.visible ? valueSelectionComboBox.currentIndex : inputTextField.text, selectedNetworkAttribute.dataType)

        if (isNaN(data)) {
            dialogText.text = qsTr("Invalid input. Please try again.")
            dialog.visible = true;
            return;
        }

        const expression = ArcGISRuntimeEnvironment.createObject("UtilityNetworkAttributeComparison", {
                                                                     networkAttribute: selectedNetworkAttribute,
                                                                     comparisonOperator: comparisonOperatorComboBox.currentIndex,
                                                                     value: data});

        const otherExpression = traceConfiguration.traversability.barriers;

        if (otherExpression) {
            const combineExpressions = ArcGISRuntimeEnvironment.createObject("UtilityTraceOrCondition", {
                                                                           leftExpression: otherExpression,
                                                                           rightExpression: expression
                                                                       });

            traceConfiguration.traversability.barriers = combineExpressions;
            expressionBuilder.text = expressionToString(combineExpressions);
        } else {
            traceConfiguration.traversability.barriers = expression;
            expressionBuilder.text = expressionToString(expression);
        }
    }

    function convertToDataType(data, dataType) {
        switch(dataType) {
        case Enums.UtilityNetworkAttributeDataTypeInteger:
            return parseInt(data, 10);
        case Enums.UtilityNetworkAttributeDataTypeFloat:
        case Enums.UtilityNetworkAttributeDataTypeDouble:
            return data;
        case Enums.UtilityNetworkAttributeDataTypeBoolean:
            return data == 1 ? true : false;
        }
    }

    function reset() {
        traceConfiguration.traversability.barriers = initialExpression;
        expressionBuilder.text = expressionToString(initialExpression);
    }

    function trace() {
        busyIndicator.visible = true;
        if (!utilityNetwork || !utilityElementStartingLocation) {
            return;
        } else {
            const startingLocations = [utilityElementStartingLocation];
            params.startingLocations = startingLocations;
            params.traceType = Enums.UtilityTraceTypeSubnetwork;
            params.traceConfiguration = traceConfiguration;

            utilityNetwork.trace(params);
        }
    }

    UtilityTraceParameters {
        id: params
    }

    Rectangle {
        anchors.fill: parent
        color: "lightgrey"

        ColumnLayout {
            id: controlItemsLayout
            anchors.left: parent.left
            anchors.right: parent.right
            CheckBox {
                text: qsTr("Include barriers")
                Layout.fillWidth: true
                checkState: Qt.Checked
                onCheckStateChanged: traceConfiguration.includeBarriers = checked;
            }

            CheckBox {
                text: qsTr("Include containers")
                Layout.fillWidth: true
                checkState: Qt.Checked
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
                Layout.fillWidth: true
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

                onCurrentTextChanged: updateInputMethod(currentText, currentIndex);
            }

            ComboBox {
                id: comparisonOperatorComboBox
                model: comparisonOperatorModel
                Layout.fillWidth: true
            }

            RowLayout {
                ComboBox {
                    id: valueSelectionComboBox
                    Layout.fillWidth: true
                    visible: !inputTextField.visible
                }

                TextField {
                    id: inputTextField
                    Layout.fillWidth: true
                    visible: true
                    validator: DoubleValidator {}
                    color: "black"
                    placeholderText: qsTr("Enter value here")
                    placeholderTextColor: "black"
                    background: Rectangle {
                        anchors.centerIn: parent
                        width: parent.width
                        color: "white"
                    }
                }
            }

            Button {
                text: qsTr("Add")
                Layout.fillWidth: true
                onClicked: addCondition();
            }

            ScrollView {
                Layout.fillWidth: true
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
                    onClicked: trace();
                }

                Button {
                    text: qsTr("Reset")
                    Layout.fillWidth: true
                    onClicked: reset();
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
