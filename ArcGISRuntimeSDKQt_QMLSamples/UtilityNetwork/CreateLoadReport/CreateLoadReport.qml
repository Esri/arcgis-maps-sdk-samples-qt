// [WriteFile Name=CreateLoadReport, Category=UtilityNetwork]
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
import Esri.ArcGISRuntime 100.11
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var utilityAssetType: null
    property var utilityTier: null
    property var startingLocation: null
    property var phasesCurrentAttr: null
    property var phaseList: []
    property var traceConfiguration: null
    property var baseCondition: null
    property var taskMap: null

    property var phases: ["A", "AB", "ABC", "AC", "B", "BC", "C", "DeEnergized", "Unknown"]

    property var selectedPhases: {
        "A": false,
        "AB": false,
        "ABC": false,
        "AC": false,
        "BC": false,
        "B": false,
        "C": false,
        "DeEnergized": false,
        "Unknown": false
    }

    property var phaseLoad: {
        "A": 0,
        "AB": 0,
        "ABC": 0,
        "AC": 0,
        "BC": 0,
        "B": 0,
        "C": 0,
        "DeEnergized": 0,
        "Unknown": 0
    }

    property var phaseCust: {
        "A": 0,
        "AB": 0,
        "ABC": 0,
        "AC": 0,
        "BC": 0,
        "B": 0,
        "C": 0,
        "DeEnergized": 0,
        "Unknown": 0
    }

    UtilityNetwork {
        id: utilityNetwork
        url: "https://sampleserver7.arcgisonline.com/server/rest/services/UtilityNetwork/NapervilleElectric/FeatureServer"

        credential: Credential {
            username: "editor01"
            password: "S7#i2LWmYH75"
        }

        Component.onCompleted: {
            utilityNetwork.load();
        }

        onLoadStatusChanged: {
            console.log(utilityNetwork.loadStatus);

            if (utilityNetwork.loadStatus === Enums.LoadStatusLoaded) {
                utilityAssetType = createUtilityAssetType();
                utilityTier = createUtilityTier();
                startingLocation = createStartingLocation();
                serviceCategoryComparison.setServiceCategoryComparison();
                baseCondition = utilityTier.traceConfiguration.traversability.barriers;
                phaseList = createPhaseList();
                addLoadAttributeFunction.setNetworkAttribute();
            }
        }
        onTraceStatusChanged: {
            console.log(traceStatus);
        }
        onErrorChanged: {
            console.log(error.message);
        }
    }

    function createPhaseList() {
        phasesCurrentAttr = utilityNetwork.definition.networkAttribute("Phases current");

        if (phasesCurrentAttr.domain.domainType === Enums.DomainTypeCodedValueDomain) {
            const codedValueDomain = phasesCurrentAttr.domain;
            return codedValueDomain.codedValues;
        }


    }

    function createUtilityAssetType() {
        const networkDefinition = utilityNetwork.definition;
        const networkSource = networkDefinition.networkSource("Electric Distribution Device");
        const assetGroup = networkSource.assetGroup("Circuit Breaker");
        return assetGroup.assetType("Three Phase");
    }

    function createUtilityTier() {
        const networkDefinition = utilityNetwork.definition;
        const domainNetwork = networkDefinition.domainNetwork("ElectricDistribution");
        return domainNetwork.tier("Medium Voltage Radial");
    }

    function createStartingLocation() {
        let loadTerminal;
        const utilityTerminals = utilityAssetType.terminalConfiguration.terminals;

        for (let i = 0; i < utilityTerminals.length; i++) {
            if (utilityTerminals[i].name === "Load") {
                loadTerminal = utilityTerminals[i];
                break;
            }
        }

        if (!loadTerminal)
            return;

        return utilityNetwork.createElementWithAssetType(utilityAssetType, "{1CAF7740-0BF4-4113-8DB2-654E18800028}", loadTerminal);
    }

    UtilityCategoryComparison {
        id: serviceCategoryComparison
        comparisonOperator: Enums.UtilityCategoryComparisonOperatorExists

        function setServiceCategoryComparison() {
            let utilityCategories = utilityNetwork.definition.categories;
            for (let i = 0; i < utilityCategories.length; i++) {
                if (utilityCategories[i].name === "ServicePoint") {
                    serviceCategoryComparison.category = utilityCategories[i];
                    break;
                }
            }
        }
    }

    UtilityTraceFunction {
        id: addLoadAttributeFunction
        functionType: Enums.UtilityTraceFunctionTypeAdd
        condition: serviceCategoryComparison

        function setNetworkAttribute() {
            addLoadAttributeFunction.networkAttribute = utilityNetwork.definition.networkAttribute("Service Load");
        }
    }

    UtilityTraceConfiguration {
        id: traceConfiguration
        outputCondition: serviceCategoryComparison
        includeBarriers: false

        Component.onCompleted: {
            traceConfiguration.functions.clear();
            traceConfiguration.functions.append(addLoadAttributeFunction);
        }
    }

    UtilityTraceParameters {
        id: traceParams
        traceType: Enums.UtilityTraceTypeDownstream
        startingLocations: [startingLocation]
        traceConfiguration: traceConfiguration
        barriers: [traceOrCondition]

        Component.onCompleted: {
            traceParams.resultTypes = [Enums.UtilityTraceResultTypeElements, Enums.UtilityTraceResultTypeFunctionOutputs]
        }
    }

    function setCodedValuesOnTraceParameters() {

    }

    UtilityNetworkAttributeComparison {
        id: networkAttributeComparison
        comparisonOperator: Enums.UtilityAttributeComparisonOperatorDoesNotIncludeAny
        otherNetworkAttribute: phasesCurrentAttr
    }

    UtilityTraceOrCondition {
        id: traceOrCondition
        leftExpression: baseCondition
        rightExpression: networkAttributeComparison
    }

    function runReport(selectedPhases) {
        let activeValues = [];
        for (let i = 0; i < phaseList.length; i++) {
            if (selectedPhases.includes(phaseList[i].name)) {
                activeValues.push(phaseList[i]);
            }
        }

        activeValues.forEach((codedValue) => {
                                 networkAttributeComparison.value = codedValue.code;
                                 taskMap = utilityNetwork.trace(traceParams);
                             });

        return;
    }

    Rectangle {
        id: rectangle
        color: "white"
        width: grid.width
        height: contents.height
        anchors {
            horizontalCenter: parent.horizontalCenter
        }

        Column {
            id: contents
            anchors.fill: parent
            padding: 10
            spacing: 25

            Row {
                GridLayout {
                    id: grid
                    columns: 4

                    Text {}
                    Text { text: "Phase"; font.pointSize: 18; font.bold: true }
                    Text { text: "Total customers"; font.pointSize: 18; font.bold: true; }
                    Text { text: "Total load"; font.bold: true; font.pointSize: 18 }

                    CheckBox { onCheckedChanged: selectedPhases["A"] = !selectedPhases["A"] }
                    Text { text: "A" }
                    Text { text: phaseCust["A"] ? phaseCust["A"] : 0 }
                    Text { text: phaseLoad["A"] ? phaseLoad["A"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["AB"] = !selectedPhases["AB"] }
                    Text { text: "AB" }
                    Text { text: phaseCust["AB"] ? phaseCust["AB"] : 0 }
                    Text { text: phaseLoad["AB"] ? phaseLoad["AB"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["ABC"] = !selectedPhases["ABC"] }
                    Text { text: "ABC" }
                    Text { text: phaseCust["ABC"] ? phaseCust["ABC"] : 0 }
                    Text { text: phaseLoad["ABC"] ? phaseLoad["ABC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["AC"] = !selectedPhases["AC"] }
                    Text { text: "AC" }
                    Text { text: phaseCust["AC"] ? phaseCust["AC"] : 0 }
                    Text { text: phaseLoad["AC"] ? phaseLoad["AC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["B"] = !selectedPhases["B"] }
                    Text { text: "B" }
                    Text { text: phaseCust["B"] ? phaseCust["B"] : 0 }
                    Text { text: phaseLoad["B"] ? phaseLoad["B"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["BC"] = !selectedPhases["BC"] }
                    Text { text: "BC" }
                    Text { text: phaseCust["BC"] ? phaseCust["BC"] : 0 }
                    Text { text: phaseLoad["BC"] ? phaseLoad["BC"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["C"] = !selectedPhases["C"] }
                    Text { text: "C" }
                    Text { text: phaseCust["C"] ? phaseCust["C"] : 0 }
                    Text { text: phaseLoad["C"] ? phaseLoad["C"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["DeEnergized"] = !selectedPhases["DeEnergized"] }
                    Text { text: "DeEnergized" }
                    Text { text: phaseCust["DeEnergized"] ? phaseCust["DeEnergized"] : 0 }
                    Text { text: phaseLoad["DeEnergized"] ? phaseLoad["DeEnergized"] : 0 }

                    CheckBox { onCheckedChanged: selectedPhases["Unknown"] = !selectedPhases["Unknown"] }
                    Text { text: "Unknown" }
                    Text { text: phaseCust["Unknown"] ? phaseCust["Unknown"] : 0 }
                    Text { text: phaseLoad["Unknown"] ? phaseLoad["Unknown"] : 0 }
                }
            }

            Row {
                Button {
                    text: "Run Report"
                    onClicked: {
                        let runPhases = [];
                        phases.forEach((phase) => {
                                           if (selectedPhases[phase])
                                           runPhases.push(phase)
                                       });
                        runReport(runPhases);
                    }
                }
            }
        }
    }
}
