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
            accessoryButtonType: addFeaturesSample.operationMode === 1 ? "Custom" : (addFeaturesSample.operationMode === 2 ? "Info" : "None")
            customImageUrl: addFeaturesSample.operationMode === 1 ? "qrc:/Samples/EditData/ManageFeaturesFeatureService/ic_menu_trash_light.png" : ""

            onAccessoryButtonClicked: {
                if (addFeaturesSample.operationMode === 1) {
                    if (callout.visible)
                        callout.dismiss();
                    addFeaturesSample.deleteSelectedFeature();
                } else if (addFeaturesSample.operationMode === 2) {
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
        color: palette.window
        border.color: palette.windowText
        border.width: 1
        radius: 5

        Column {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
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
                    case 0: return "Click anywhere on the map to add new features"
                    case 1: return "Click on an existing feature to select and delete it"
                    case 2: return "Click on an existing feature to select and update its attributes"
                    case 3: return "Click to select a feature, then click again to move it to a new location"
                    default: return ""
                    }
                }
            }
        }
    }

    // Handle feature selection for callout modes
    onFeatureSelected: {
        if (addFeaturesSample.operationMode === 1 || addFeaturesSample.operationMode === 2) {
            // show the callout
            callout.showCallout();
            
            if (addFeaturesSample.operationMode === 2) {
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
        color: palette.window
        border.color: palette.windowText
        border.width: 1
        z: 200

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
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
