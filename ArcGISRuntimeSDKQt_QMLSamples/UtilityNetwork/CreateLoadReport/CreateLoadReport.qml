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
    property var baseCondition: null

    property var traceOrCondition: null
    property var traceConfiguration: null

    property bool reportHasRun: false
    property var sampleStatus: 0

    property var phaseQueue: []
    property var currentPhase: null

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

    property var phaseLoad: ({})

    property var phaseCust: ({})



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

                traceConfiguration = createTraceConfiguration();
                traceParams.traceConfiguration = traceConfiguration;
                baseCondition = traceConfiguration.traversability.barriers;

                phaseList = createPhaseList();
                sampleStatus = 2;
            }
        }

        onTraceStatusChanged: {
            console.log(traceStatus);
            if (traceStatus === Enums.TaskStatusCompleted) {
                console.log("trace result count: "+ traceResult.count);
                console.log(currentPhase.name);
                traceResult.forEach(result => {
                                        if (result.objectType === "UtilityElementTraceResult") {
                                            const objectIds = [];
                                            const elements = result.elements;
                                            for (let i = 0; i < elements.length; i++) {
                                                if (!objectIds.includes(elements[i].objectId)) {
                                                    objectIds.push(elements[i].objectId);
                                                }
                                            }
                                            phaseCust[currentPhase.name] = objectIds.length;
                                            console.log("oids: " + objectIds.length);
                                        } else if (result.objectType === "UtilityFunctionTraceResult") {
                                            phaseLoad[currentPhase.name] = result.functionOutputs[0].result;
                                            console.log("load: " + result.functionOutputs[0].result);
                                        }
                                    });

            if (phaseQueue.length > 0) {
                currentPhase = phaseQueue.shift();
                createReportForPhase(currentPhase);
            } else {
                sampleStatus = 2;
                reportHasRun = true;
            }
            }
            if (traceStatus === Enums.TaskStatusErrored) {
                sampleStatus = -1;
            }
        }
        onErrorChanged: {
            console.log(error.message);
            console.log(error.additionalMessage);
            sampleStatus = -1;
        }
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

        onErrorChanged: {
            console.log(error.message);
        }
    }

    UtilityTraceFunction {
        id: addLoadAttributeFunction
        functionType: Enums.UtilityTraceFunctionTypeAdd
        condition: serviceCategoryComparison

        function setNetworkAttribute() {
            addLoadAttributeFunction.networkAttribute = utilityNetwork.definition.networkAttribute("Service Load");
        }

        onErrorChanged: {
            console.log(error.message);
        }
    }

    UtilityTraceParameters {
        id: traceParams
        traceType: Enums.UtilityTraceTypeDownstream
        startingLocations: [startingLocation]

        onErrorChanged: {
            console.log(error.message);
        }

        Component.onCompleted: {
            traceParams.resultTypes = [Enums.UtilityTraceResultTypeElements, Enums.UtilityTraceResultTypeFunctionOutputs]
        }
    }

    UtilityNetworkAttributeComparison {
        id: networkAttributeComparison
        comparisonOperator: Enums.UtilityAttributeComparisonOperatorDoesNotIncludeAny
        otherNetworkAttribute: phasesCurrentAttr
        onErrorChanged: {
            console.log(error.message);
        }
        Component.onCompleted: {
            console.log(networkAttributeComparison);
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

    function createTraceConfiguration() {
        let traceConfig = utilityTier.traceConfiguration;
        traceConfig.domainNetwork = utilityNetwork.definition.domainNetwork("ElectricDistribution");

        serviceCategoryComparison.setServiceCategoryComparison();
        traceConfig.outputCondition = serviceCategoryComparison;

        addLoadAttributeFunction.setNetworkAttribute();
        traceConfig.functions.clear();
        traceConfig.functions.append(addLoadAttributeFunction);

        traceConfig.includeBarriers = false;
        return traceConfig;
    }

    function createPhaseList() {
        phasesCurrentAttr = utilityNetwork.definition.networkAttribute("Phases current");

        if (phasesCurrentAttr.domain.domainType === Enums.DomainTypeCodedValueDomain) {
            const codedValueDomain = phasesCurrentAttr.domain;
            return codedValueDomain.codedValues;
        }
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

    function runReport(selectedPhases) {
        for (let i = 0; i < phaseList.length; i++) {
            if (selectedPhases.includes(phaseList[i].name)) {
                phaseQueue.push(phaseList[i]);
            }
        }

        if (phaseQueue.length > 0) {
            sampleStatus = 1;
            currentPhase = phaseQueue.shift();
            createReportForPhase(currentPhase);
        }
        return;
    }

    function createReportForPhase(phase) {
        const condExpr = ArcGISRuntimeEnvironment.createObject("UtilityNetworkAttributeComparison", {
                                                                   networkAttribute: phasesCurrentAttr,
                                                                   comparisonOperator: Enums.UtilityAttributeComparisonOperatorDoesNotIncludeAny,
                                                                   value: phase.code});

        traceOrCondition = ArcGISRuntimeEnvironment.createObject("UtilityTraceOrCondition", {
                                                                     leftExpression: baseCondition,
                                                                     rightExpression: condExpr
                                                                 });
        traceParams.traceConfiguration.traversability.barriers = traceOrCondition;
        console.log("bars:" + traceParams.traceConfiguration.traversability.barriers);
        utilityNetwork.trace(traceParams);
    }

    Rectangle {
        id: rectangle
        color: "white"
        width: grid.width
        height: contents.height
        anchors {
            horizontalCenter: parent.horizontalCenter

        }

        ButtonGroup {
            id: checkBoxes
            exclusive: false
            checkState: parentBox.checkState
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
                    columnSpacing: 50
                    rowSpacing: 5

                    CheckBox { id: parentBox; checkState: checkBoxes.checkState }
                    Text { text: "Phase"; font.pointSize: 18; font.bold: true }
                    Text { text: "Total customers"; font.pointSize: 18; font.bold: true; }
                    Text { text: "Total load"; font.bold: true; font.pointSize: 18 }

                    CheckBox { id: checkA; onCheckedChanged: selectedPhases["A"] = !selectedPhases["A"]; ButtonGroup.group: checkBoxes }
                    Text { text: "A" }
                    Text { text: "A" in phaseCust ? phaseCust["A"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "A" in phaseLoad ? phaseLoad["A"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkAB; onCheckedChanged: selectedPhases["AB"] = !selectedPhases["AB"]; ButtonGroup.group: checkBoxes }
                    Text { text: "AB" }
                    Text { text: "AB" in phaseCust ? phaseCust["AB"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "AB" in phaseLoad ? phaseLoad["AB"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkABC; onCheckedChanged: selectedPhases["ABC"] = !selectedPhases["ABC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "ABC" }
                    Text { text: "ABC" in phaseCust ? phaseCust["ABC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "ABC" in phaseLoad ? phaseLoad["ABC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkAC; onCheckedChanged: selectedPhases["AC"] = !selectedPhases["AC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "AC" }
                    Text { text: "AC" in phaseCust ? phaseCust["AC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "AC" in phaseLoad ? phaseLoad["AC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkB; onCheckedChanged: selectedPhases["B"] = !selectedPhases["B"]; ButtonGroup.group: checkBoxes }
                    Text { text: "B" }
                    Text { text: "B" in phaseCust ? phaseCust["B"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "B" in phaseLoad ? phaseLoad["B"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkBC; onCheckedChanged: selectedPhases["BC"] = !selectedPhases["BC"]; ButtonGroup.group: checkBoxes }
                    Text { text: "BC" }
                    Text { text: "BC" in phaseCust ? phaseCust["BC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "BC" in phaseLoad ? phaseLoad["BC"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkC; onCheckedChanged: selectedPhases["C"] = !selectedPhases["C"]; ButtonGroup.group: checkBoxes }
                    Text { text: "C" }
                    Text { text: "C" in phaseCust ? phaseCust["C"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "C" in phaseLoad ? phaseLoad["C"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkDeEnergized; onCheckedChanged: selectedPhases["DeEnergized"] = !selectedPhases["DeEnergized"]; ButtonGroup.group: checkBoxes }
                    Text { text: "DeEnergized" }
                    Text { text: "DeEnergized" in phaseCust ? phaseCust["DeEnergized"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "DeEnergized" in phaseLoad ? phaseLoad["DeEnergized"].toLocaleString(Qt.locale(), "f", 0) : "NA" }

                    CheckBox { id: checkUnknown; onCheckedChanged: selectedPhases["Unknown"] = !selectedPhases["Unknown"]; ButtonGroup.group: checkBoxes }
                    Text { text: "Unknown" }
                    Text { text: "Unknown" in phaseCust ? phaseCust["Unknown"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                    Text { text: "Unknown" in phaseLoad ? phaseLoad["Unknown"].toLocaleString(Qt.locale(), "f", 0) : "NA" }
                }
            }

            Row {
                Button {
                    text: {
                        if (checkBoxes.checkState !== 0 || !reportHasRun) {
                            "Run Report";
                        } else {
                            "Reset";
                        }
                    }

                    enabled: ((reportHasRun || checkBoxes.checkState !== 0) && sampleStatus === 2) ? true : false

                    onClicked: {
                        let runPhases = [];
                        phases.forEach((phase) => {
                                           if (selectedPhases[phase])
                                           runPhases.push(phase)
                                       });
                        runReport(runPhases);

                        if (runPhases.length === 0) {
                            reportHasRun = false;
                        } else {
                            reportHasRun = true;
                        }
                    }
                }
            }

            Row {
                Text {
                    id: noticeText
                    text: {
                        switch (sampleStatus) {
                        case -1:
                            "Error initializing sample";
                            break;
                        case 0:
                            "Sample initializing...";
                            break;
                        case 1:
                            "Generating load report...";
                            break;
                        case 2:
                            if (checkBoxes.checkState === 0 && !reportHasRun) {
                                "Select phases to run the load report with";
                            } else {
                                "Tap the \"Run Report\" button to create the load report";
                            }
                            break;
                        default:
                            "";
                            break;
                        }
                    }
                }
            }
        }
    }

}
