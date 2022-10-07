// [WriteFile Name=EditWithBranchVersioning, Category=EditData]
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

import QtQuick
import Esri.ArcGISRuntime

import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Esri.ArcGISRuntime.Toolkit

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    readonly property var featAttributes: ["Affected", "Destroyed", "Inaccessible", "Minor", "Major"]
    readonly property var versionAccessModel: ["Public", "Protected", "Private"]
    property var serviceFeatureTable: null
    property var featureLayer: null
    property var selectedFeature: null
    property string featureName: ""
    property string currentTypeDamage: ""
    property string currentSgdbVersion: ""
    property string createdVersionName: ""
    property bool sgdbVersionIsDefault: sgdb.defaultVersionName === sgdb.versionName ? true : false

    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        onViewpointChanged: {
            clearSelectedFeature();
            callout.dismiss();
        }

        onMouseClicked: mouse => {
            busyIndicator.visible = true;
            featureLayer.clearSelection();

            // if feature is already selected, then move to new location
            if (selectedFeature) {
                const clickedPoint = mapView.screenToLocation(mouse.x, mouse.y);
                moveFeature(clickedPoint);
            } else {
                // call identify on the map view
                mapView.identifyLayerWithMaxResults(featureLayer, mouse.x, mouse.y, 5, false, 1);
            }
        }

        onIdentifyLayerStatusChanged: {
            if (identifyLayerStatus === Enums.TaskStatusCompleted) {
                // clear any previous selections
                clearSelectedFeature();

                if (identifyLayerResult.geoElements.length > 0) {
                    selectedFeature = identifyLayerResult.geoElements[0];
                    featureLayer.selectFeature(selectedFeature);
                } else {
                    busyIndicator.visible = false;
                    return;
                }

                featureName = selectedFeature.attributes.attributeValue("PLACENAME");
                currentTypeDamage = selectedFeature.attributes.attributeValue("TYPDAMAGE");

                callout.showCallout();
                busyIndicator.visible = false;
            }
        }

        calloutData {
            // HTML to style the title text by centering it, increase pt size,
            // and bolding it.
            title: "<br><font size=\"+2\">%1</font>".arg(featureName)
            location: selectedFeature ? selectedFeature.geometry : null
        }

        Callout {
            id: callout
            background: Rectangle {
                border.width: 1
                border.color: "lightgrey"
            }
            calloutData: parent.calloutData
            leaderPosition: Callout.LeaderPosition.Automatic
            onAccessoryButtonClicked: {
                for (let i=0; i < featAttributes.length; i++) {
                    if (currentTypeDamage === featAttributes[i]) {
                        typeDmgCombo.currentIndex = i;
                        break;
                    }
                }
                updateWindow.visible = true;
            }
        }

        Map {
            Basemap {
                initStyle: Enums.BasemapStyleArcGISStreets
            }
        }
    }

    ServiceGeodatabase {
        id: sgdb
        url: "https://sampleserver7.arcgisonline.com/server/rest/services/DamageAssessment/FeatureServer"

        credential: Credential {
            username: "editor01"
            password: "S7#i2LWmYH75"
        }

        onLoadStatusChanged: {
            if (loadStatus !== Enums.LoadStatusLoaded)
                return;

            currentSgdbVersion = sgdb.versionName;
            serviceFeatureTable = sgdb.table(0);
            serviceFeatureTable.updateFeatureStatusChanged.connect(()=> {
                if (serviceFeatureTable.updateFeatureStatus === Enums.TaskStatusCompleted)
                    busyIndicator.visible = false;
             });

            serviceFeatureTable.loadStatusChanged.connect(()=> {
                if (serviceFeatureTable.loadStatus !== Enums.LoadStatusLoaded)
                    return;

                mapView.setViewpointGeometry(featureLayer.fullExtent);
                busyIndicator.visible = false;
            });

            featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer");
            featureLayer.featureTable = serviceFeatureTable;
            mapView.map.operationalLayers.append(featureLayer);
        }

        onCreateVersionStatusChanged: {
            if (createVersionStatus === Enums.TaskStatusCompleted) {
                createdVersionName = createVersionResult.name;
                createVersionBtn.text = qsTr("Switch to default version");
                switchVersions();
            } else if (createVersionStatus === Enums.TaskStatusErrored) {
                errorText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
                errorDialog.visible = true;
                busyIndicator.visible = false;
            }
        }

        onSwitchVersionStatusChanged: {
            if (switchVersionStatus === Enums.TaskStatusCompleted) {
                currentSgdbVersion = sgdb.versionName;
            } else if (switchVersionStatus === Enums.TaskStatusErrored) {
                errorText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
                errorDialog.visible = true;
            }
            busyIndicator.visible = false;
        }

        onApplyEditsStatusChanged: {
            if (applyEditsStatus === Enums.TaskStatusCompleted) {
                switchVersions();
                applyEditsDialog.visible = false;

            } else if (applyEditsStatus === Enums.TaskStatusErrored) {
                errorText.text = qsTr("%1 - %2".arg(error.message).arg(error.additionalMessage));
                errorDialog.visible = true;
                return;
            }
        }

        onComponentCompleted: {
            load();
            busyIndicator.visible = true;
        }
    }

    ServiceVersionParameters {
        id: params
    }

    Button {
        id: createVersionBtn
        text: qsTr("Create Version")
        anchors {
            left: parent.left
            top: parent.top
            margins: 3
        }
        enabled: !busyIndicator.visible

        onClicked: {
            if (text === qsTr("Create Version")) {
                createVersionWindow.visible = true;
                callout.dismiss();
                updateWindow.visible = false;
            } else if (text === qsTr("Switch to default version")) {
                text = qsTr("Switch to created version")
                busyIndicator.visible = true;

                if (sgdb.hasLocalEdits()) {
                    sgdb.applyEdits();
                    applyEditsDialog.visible = true;
                } else {
                    switchVersions();
                }
                callout.dismiss();
                updateWindow.visible = false;
            } else if (text === qsTr("Switch to created version")) {
                text = qsTr("Switch to default version")
                switchVersions();
                callout.dismiss();
                updateWindow.visible = false;
            }
            clearSelectedFeature();
        }
    }

    Rectangle {
        id: currentVersionRect
        anchors{
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 3
        }
        width: childrenRect.width;
        height: childrenRect.height;
        color: "#000000"
        visible: currentVersionText.text !== "" ? true : false

        ColumnLayout {
            spacing: 3
            Text {
                text: qsTr("Current version:")
                Layout.alignment: Qt.AlignHCenter
                color: "white"
            }

            Text {
                id: currentVersionText
                Layout.alignment: Qt.AlignHCenter
                text: currentSgdbVersion
                color: "white"
            }
        }
    }

    Rectangle {
        id: createVersionWindow
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
        radius: 10
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        ColumnLayout {
            anchors.margins: 5

            TextField {
                id: versionNameTextField
                placeholderText: qsTr("Name must be unique")
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
                validator: RegularExpressionValidator { regularExpression: /\w{0,50}/ }
            }

            ComboBox {
                id: accessComboBox
                model: versionAccessModel
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
            }

            TextField {
                id: descriptionTextField
                placeholderText: qsTr("Enter description")
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
            }

            Row {
                Layout.alignment: Qt.AlignRight
                Layout.margins: 5
                height: childrenRect.height
                spacing: 5

                Button {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignRight
                    text: qsTr("Create")
                    onClicked: {
                        createVersion(versionNameTextField.text, accessComboBox.currentValue, descriptionTextField.text);
                        busyIndicator.visible = true;
                        resetCreateVersionWindow();
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    onClicked: {
                        resetCreateVersionWindow();
                    }
                }
            }
        }
    }

    // Update Window
    Rectangle {
        id: updateWindow
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
        radius: 10
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        GridLayout {
            columns: 2
            anchors.margins: 5

            Text {
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Update Attributes")
                font.pixelSize: 16
            }

            Text {
                text: "TYPDAMAGE:"
                Layout.margins: 5
            }

            ComboBox {
                id: typeDmgCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.margins: 5
                Layout.fillWidth: true
                model: featAttributes
                enabled: !sgdbVersionIsDefault
            }

            Row {
                Layout.alignment: Qt.AlignRight
                Layout.columnSpan: 2
                height: childrenRect.height
                spacing: 5

                Button {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignRight
                    text: qsTr("Update")
                    enabled: !sgdbVersionIsDefault
                    // once the update button is clicked, hide the windows, and fetch the currently selected features
                    onClicked: {
                        updateAttribute(typeDmgCombo.currentValue);
                        updateWindow.visible = false;
                        callout.dismiss();
                        busyIndicator.visible = true;
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    // once the cancel button is clicked, hide the window
                    onClicked: {
                        clearSelectedFeature();
                        updateWindow.visible = false;
                        callout.dismiss();
                    }
                }
            }
        }
    }

    function switchVersions() {
        if (!sgdbVersionIsDefault) {
            sgdb.switchVersion(sgdb.defaultVersionName);
            createVersionBtn.text = qsTr("Switch to created version");
        } else {
            sgdb.switchVersion(createdVersionName);
        }
    }

    function moveFeature(mapPoint) {
        if (!sgdbVersionIsDefault) {
            selectedFeature.geometry = mapPoint;
            selectedFeature.featureTable.updateFeature(selectedFeature);
        }
        busyIndicator.visible = false;
        clearSelectedFeature();
    }

    function clearSelectedFeature() {
        selectedFeature = null;
        if (featureLayer)
            featureLayer.clearSelection();

        callout.dismiss();
    }

    function createVersion(versionName, versionAccess, description) {
        params.name = versionName;
        params.description = description;

        if (versionAccess === "Private") {
            params.access = Enums.VersionAccessPrivate;
        } else if (versionAccess === "Protected") {
            params.access = Enums.VersionAccessProtected;
        } else if (versionAccess == "Public") {
            params.access = Enums.VersionAccessPublic;
        }
        sgdb.createVersion(params);
    }

    function resetCreateVersionWindow() {
        createVersionWindow.visible = false;
        versionNameTextField.text = "";
        descriptionTextField.text = "";
        accessComboBox.currentIndex = 0;
    }

    function updateAttribute(attributeValue) {
        busyIndicator.visible = true;
        if (!selectedFeature)
            return;

        if (sgdbVersionIsDefault) {
            clearSelectedFeature();
            return;
        }

        // update the attirbute with the selection from the combo box
        selectedFeature.attributes.replaceAttribute("TYPDAMAGE", attributeValue);
        selectedFeature.featureTable.updateFeature(selectedFeature);
    }

    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
        anchors.fill: parent
    }

    Dialog {
        id: errorDialog
        anchors.centerIn: parent
        standardButtons: Dialog.Ok

        Text {
            id: errorText
        }
    }

    Dialog {
        id: applyEditsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) - mapView.attributionRect.height;

        Text {
            id: applyEditsText
            text: qsTr("Applying Edits")
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
    }
}
