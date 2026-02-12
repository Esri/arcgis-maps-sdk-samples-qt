// [WriteFile Name=ManageFeaturesFeatureService, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

ManageFeaturesFeatureServiceSample {
    id: addFeaturesSample
    width: 800
    height: 600

    readonly property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]
    readonly property var operationModes: ["Add Features", "Delete Features", "Update Attributes", "Update Geometry"]

    // MapView component
    MapView {
        id: view
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Callout for Delete and Update Attributes modes
        Callout {
            id: callout
            calloutData: view.calloutData
            leaderPosition: Callout.LeaderPosition.Automatic
            
            // Delete mode - show trash button, Update mode - show accessory button
            accessoryButtonType: addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.DeleteFeatures ? "Custom" :
                                                                                                                                 (addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.UpdateAttributes ? "Info" : "None")

            customImageUrl: addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.DeleteFeatures ? "qrc:/Samples/EditData/ManageFeaturesFeatureService/trash-24.svg" : ""

            onAccessoryButtonClicked: {
                if (addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.DeleteFeatures) {
                    if (callout.visible)
                        callout.dismiss();
                    addFeaturesSample.deleteSelectedFeature();
                } else if (addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.UpdateAttributes) {
                    updateWindow.visible = true;
                }
            }
        }
    }

    // Text metrics to measure longest option
    TextMetrics {
        id: textMetrics
        font.pixelSize: 12
        text: qsTr("Update Attributes")
    }

    // Simple right panel - only takes space it needs
    Rectangle {
        id: rightPanel
        anchors {
            top: parent.top
            right: parent.right
            margins: 10
        }
        width: Math.max(textMetrics.width + 60, 200) + 20
        height: instructionText.visible ? 100 : 55
        border.width: 1
        radius: 5
        color: palette.base

        // Mouse/Touch area to prevent mouse events from propogating through the UI
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.AllButtons
            propagateComposedEvents: false

            onWheel: {
            }
        }

        MultiPointTouchArea {
            anchors.fill: parent
        }

        Column {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 10
            }
            spacing: 5

            ComboBox {
                id: modeComboBox
                width: parent.width
                height: 35
                model: operationModes
                currentIndex: addFeaturesSample.operationMode

                onCurrentIndexChanged: {
                    addFeaturesSample.operationMode = currentIndex
                }
            }

            Label {
                id: instructionText
                width: parent.width
                font.pixelSize: 11
                wrapMode: Text.WordWrap
                text: {
                    switch(addFeaturesSample.operationMode) {
                    case ManageFeaturesFeatureService.OperationMode.AddFeatures:
                        return qsTr("Click anywhere on the map to add new features")
                    case ManageFeaturesFeatureService.OperationMode.DeleteFeatures:
                        return qsTr("Click on an existing feature to select and delete it")
                    case ManageFeaturesFeatureService.OperationMode.UpdateAttributes:
                        return qsTr("Click on an existing feature to select and update its attributes")
                    case ManageFeaturesFeatureService.OperationMode.UpdateGeometry:
                        return qsTr("Click to select a feature, then click again to move it to a new location")
                    default:
                        return qsTr("")
                    }
                }
            }
        }
    }

    // Handle feature selection for callout modes
    onFeatureSelected: {
        if (addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.DeleteFeatures || addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.UpdateAttributes) {
            // Show the callout
            callout.showCallout();
            
            if (addFeaturesSample.operationMode === ManageFeaturesFeatureService.OperationMode.UpdateAttributes) {
                // Set the combo box's default value for update attributes mode
                damageComboBox.currentIndex = featAttributes.indexOf(addFeaturesSample.featureType);
            }
        }
    }

    onHideWindow: {
        // Hide the callout
        if (callout.visible)
            callout.dismiss();
        // Hide the update window
        updateWindow.visible = false;
    }

    // Simple Update Attributes Window
    Rectangle {
        id: updateWindow
        width: 360
        height: 180
        anchors.centerIn: parent
        radius: 8
        visible: false
        border.width: 1
        z: 200
        color: palette.base

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Label {
                text: qsTr("Update Feature Attribute")
                font.pixelSize: 16
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Column {
                width: parent.width
                spacing: 8

                Label {
                    text: qsTr("Damage Type:")
                    font.pixelSize: 12
                    font.bold: true
                }

                ComboBox {
                    id: damageComboBox
                    width: parent.width
                    height: 35
                    model: featAttributes
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Button {
                    text: qsTr("Update")
                    width: 100
                    height: 35
                    
                    onClicked: {
                        if (callout.visible)
                            callout.dismiss();
                        updateWindow.visible = false;
                        addFeaturesSample.updateSelectedFeature(damageComboBox.currentText)
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    width: 100
                    height: 35
                    
                    onClicked: updateWindow.visible = false;
                }
            }
        }
    }
}
