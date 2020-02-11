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

    property alias myModel: myModel //* rename
    property alias comparisonSourceListModel: comparisonSourceListModel
    readonly property var comparisonOperatorModel: ["Equal","NotEqual","GreaterThan","GreaterThanEqual","LessThan","LessThanEqual","IncludesTheValues","DoesNotIncludeTheValues","IncludesAny","DoesNotIncludeAny"]
    readonly property string deviceTableName: "Electric Distribution Device"
    readonly property string assetGroupName: "Circuit Breaker"
    readonly property string assetTypeName: "Three Phase"
    readonly property string globalId: "{1CAF7740-0BF4-4113-8DB2-654E18800028}"
    readonly property string domainNetworkName: "ElectricDistribution"
    readonly property string tierName: "Medium Voltage Radial"
    property var traceConfiguration
    property var utilityElementStartingLocation
    property var initialExpression
    property var networkDefinition
    property var currentComparisonOperator
    property var combineExpression

    property var otherExpression
    property var expression1

    property int inc: 0 //* remove
    readonly property url featureLayerUrl: "https://sampleserver7.arcgisonline.com/arcgis/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"

    ListModel {
        id: myModel //* rename
    }

    ListModel {
        id: comparisonSourceListModel //* rename networkAttributesListModel
    }

    ListModel {
        id: valueSelectionListModel
    }



    UtilityNetwork {
        id: utilityNetwork
        url: featureLayerUrl

        onTraceStatusChanged: {

        }

        onErrorChanged: {
            dialogText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
        }

        onLoadStatusChanged: {
            if(loadStatus !== Enums.LoadStatusLoaded)
                return;

            print("Matrix Reloaded"); //* remove

            networkDefinition = utilityNetwork.definition;

            for (let i = 0; i < definition.networkAttributes.length; i++) {
//                print("i:%1 name:%2 systemDefined:%3".arg(i).arg(networkDefinition.networkAttributes[i].name).arg(utilityNetwork.definition.networkAttributes[i].systemDefined)) //* remove

                if (!networkDefinition.networkAttributes[i].systemDefined)
                    comparisonSourceListModel.append({name: "%1".arg(networkDefinition.networkAttributes[i].name)});
            }

            var networkSource = networkDefinition.networkSource(deviceTableName);
            var assetGroup = networkSource.assetGroup(assetGroupName);
            var assetType = assetGroup.assetType(assetTypeName);

            utilityElementStartingLocation = utilityNetwork.createElementWithAssetType(assetType, globalId);

            var terminals = utilityElementStartingLocation.assetType.terminalConfiguration.terminals;

            for (let i = 0; i < terminals.length; i++) {
                print(terminals[i].name); //* remove
                let terminal = terminals[i];
                if (terminal.name === "Load")
                    utilityElementStartingLocation.terminal = terminal;
            }

            var domainNetwork = networkDefinition.domainNetwork(domainNetworkName);
            var utilityTierSource = domainNetwork.tier(tierName);

            traceConfiguration = utilityTierSource.traceConfiguration;

            initialExpression = traceConfiguration.traversability.barriers;

            var traceConditionType = initialExpression.traceConditionType;

            traceConfiguration.traversability.scope = Enums.UtilityTraversabilityScopeJunctions;

            let expression = expressionToString(initialExpression);
            print(expression); //* remove

            myModel.append({condition: expression});

        }

        onComponentCompleted: load();
    }

    function expressionToString(initialExpression) {

        var traceConditionType = initialExpression.traceConditionType;
        print(traceConditionType); //* remove
        var networkAttribute = initialExpression.networkAttribute;
//        print(networkAttribute.name); //* remove
        var otherNetworkAttribute = initialExpression.otherNetworkAttribute;
//        print(otherNetworkAttribute.name); //* remove

        switch (traceConditionType) {
        case Enums.UtilityTraceConditionTypeUtilityNetworkAttributeComparison:
            print("UtilityTraceConditionTypeUtilityNetworkAttributeComparison"); //* remove
            if (networkAttribute.domain) {
                if (networkAttribute.domain.domainType === Enums.DomainTypeCodedValueDomain) {
                    print("codedvaluedomain"); //* remove
                    let dataType = networkAttribute.dataType;
                    let domain = networkAttribute.domain;
                    return "`%1` %2 `%3`".arg(networkAttribute.name).arg(comparisonOperatorToString(initialExpression.comparisonOperator)).arg(domain.codedValues[initialExpression.value].name);
                }
            } else {
                return "`%1` %2 `%3`".arg(networkAttribute.name).arg(comparisonOperatorToString(initialExpression.comparisonOperator)).arg(otherNetworkAttribute? otherNetworkAttribute.name : initialExpression.value)
            }
        case Enums.UtilityTraceConditionTypUtilityCategoryComparisone:
            print("UtilityTraceConditionTypeUtilityCategoryComparisone"); //* remove
            return "`%1` %2".arg(networkAttribute.name).arg(comparisonOperatorToString(initialExpression.comparisonOperator));
        case Enums.UtilityTraceConditionTypeUtilityTraceAndCondition:
            print("UtilityTraceConditionTypeUtilityTraceAndCondition"); //* remove
            return "`%1` AND\n `%3`".arg(expressionToString(initialExpression.leftExpression)).arg(initialExpression.rightExpression);
        case Enums.UtilityTraceConditionTypeUtilityTraceOrCondition:
            print("UtilityTraceConditionTypeUtilityTraceOrCondition"); //* remove
            var temp = initialExpression.leftExpression;
            print(temp.networkAttribute.name);
            return "`%1` OR\n `%3`".arg(expressionToString(initialExpression.leftExpression)).arg(initialExpression.rightExpression);
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
        // in what situation is this needed?
        if (traceConfiguration === null) {
            traceConfiguration = ArcGISRuntimeEnvironment.createObject("UtilityTraceConfiguration");
        }
        if (traceConfiguration.traversability === null) {
            traceConfiguration.traversability = ArcGISRuntimeEnvironment.createObject("UtilityTraversability");
        }
        //*

        let workingNetworkAttribute = networkDefinition.networkAttribute(networkAttributeComboBox.currentText);

        if (workingNetworkAttribute.domain.domainType === Enums.DomainTypeCodedValueDomain) {
            print("DomainTypeCodedValueDomain"); //* remove

            expression1 = ArcGISRuntimeEnvironment.createObject("UtilityNetworkAttributeComparison", {
                                                                   networkAttribute: workingNetworkAttribute,
                                                                   comparisonOperator: comparisonOperatorComboBox.currentIndex,
                                                                   value: valueSelectionComboBox.currentIndex});

            otherExpression = traceConfiguration.traversability.barriers;

            if (otherExpression) {
                combineExpression = ArcGISRuntimeEnvironment.createObject("UtilityTraceOrCondition", {
                                                                                  leftExpression: otherExpression,
                                                                                  rightExpression: expression1
                                                                              });

                var tmp1 = combineExpression;

                var temp3 = expression1;
                traceConfiguration.traversability.barriers = combineExpression;
//                traceConfiguration.traversability.barriers = expression1;
//                print(expressionToString(traceConfiguration.traversability.barriers));

                var tmp2 = combineExpression.leftExpression;

                print(expressionToString(combineExpression));

//                combineExpression = ArcGISRuntimeEnvironment.createObject("UtilityTraceOrCondition", {
//                                                                                  leftExpression: otherExpression,
//                                                                                  rightExpression: expression1
//                                                                              });
//                traceConfiguration.traversability.barriers = combineExpression;
//                print(expressionToString(traceConfiguration.traversability.barriers));
            }

        } else {
            let expression = ArcGISRuntimeEnvironment.createObject("", {});
        }

    }

    UtilityTraceParameters {
        id: params
    }

    Rectangle {
        anchors.fill: parent
        color: "white"

        ColumnLayout {
            id: controlItemsLayout
            anchors.left: parent.left
            anchors.right: parent.right
            CheckBox {
                text: qsTr("Include barriers:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            CheckBox {
                text: qsTr("Include containers:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            ComboBox {
                //                model: ["One", "Two", "Three"] // test model
                id: networkAttributeComboBox
                model: comparisonSourceListModel ? comparisonSourceListModel : null
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                textRole: comparisonSourceListModel ? "name" : null

                onModelChanged: currentIndex = 0;

                onCurrentTextChanged: {
                    if(networkDefinition) {
                        let workingDomain = networkDefinition.networkAttribute(currentText).domain;
                        if (workingDomain) {
                            print("domain: %1".arg(workingDomain.domainType)); //* remove
                            print(workingDomain.codedValues.length); //* remove
                            valueSelectionListModel.clear();
                            for(let i = 0; i < workingDomain.codedValues.length; i++) {
                                print(workingDomain.codedValues[i].name + " - " + workingDomain.codedValues[i].code); //* remove
                                valueSelectionListModel.append({value: workingDomain.codedValues[i].name})

                            }
                            valueSelectionComboBox.model = valueSelectionListModel;
                            valueSelectionComboBox.currentIndex = 0;

                            inputTextField.visible = false;


                        } else {
                            inputTextField.visible = true;
                        }



                        print(networkDefinition.networkAttribute(currentText).name);
                    }
                }
            }

            ComboBox {
                //                model: ["Four", "TFive", "Six"] // test model
                id: comparisonOperatorComboBox
                model: comparisonOperatorModel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }

            RowLayout {
                ComboBox {
                    //                    model: ["One", "Two", "Three"]  // test model
                    id: valueSelectionComboBox
                    Layout.fillWidth: true
                    visible: !inputTextField.visible

//                    onModelChanged: currentIndex = 0;
//                    onVisibleChanged: {
//                        if (visible)
//                            currentIndex = 0;
//                    }
                }

                TextField {
                    id: inputTextField
                    Layout.fillWidth: true
                    visible: true
                }
            }

            Button {
                text: qsTr("Add")
                Layout.fillWidth: true
                onClicked: {
//                    myModel.append({condition: "`Operation Device Status` Equal `Open` %1".arg(inc++)});
                    addCondition();
                    print("appended");
                }
            }

            //            Rectangle {
            //                id: borderBoxForListView
            //                Layout.fillWidth: true
            //                Layout.minimumHeight: 150
            //                color: "transparent"
            //                Layout.margins: 2
            //                border {
            //                    color: "black"
            //                    width:  1
            //                }

            ListView {
                id: groupingView
                Layout.fillWidth: true
                Layout.minimumHeight: 150
                //                    Layout.fillHeight: true
                //                    Layout.fillWidth: true

                //                anchors {
                //                    fill: parent
                //                    margins: 5
                //                }
                //                highlight: highlightRectangle
                //                highlightResizeVelocity: 1000000
                //                highlightMoveVelocity: 1000000

                clip: true
                model: myModel
                delegate: Item {
                    width: parent.width
                    height: 25

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 10
                        Text {
                            text: "(%1)".arg(condition)
                            font.pixelSize: 12
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: groupingView.currentIndex = index;
                    }
                }

                onCurrentIndexChanged: {
                    print("currentIndex: %1".arg(currentIndex));
                }
            }

            //            }

            RowLayout {
                Button {
                    text: qsTr("Trace")
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Reset")
                    Layout.fillWidth: true
                }
            }

            Button {
                id: testButton
                text: qsTr("test input text field to comboBox")
//                onClicked: inputTextField.visible = !inputTextField.visible
                onClicked: {
//                    let workingNetworkAttribute = networkDefinition.networkAttribute(networkAttributeComboBox.currentText);
//                    print(workingNetworkAttribute.domain.codedValues[valueSelectionComboBox.currentIndex].code);
//                    print(valueSelectionComboBox.currentIndex);
                }
            }
        }
    }
}
