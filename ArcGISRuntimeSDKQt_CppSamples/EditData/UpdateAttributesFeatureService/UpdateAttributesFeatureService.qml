// [WriteFile Name=UpdateAttributesFeatureService, Category=EditData]
// [Legal]
// Copyright 2016 Esri.

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
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

UpdateAttributesFeatureServiceSample {
    id: updateFeaturesSample
    width: 800
    height: 600

    readonly property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            id: callout
            background: Rectangle {
                border.width: 1;
                border.color: "lightgrey"
            }
            calloutData: mapView.calloutData
            leaderPosition: Callout.LeaderPosition.Automatic
            onAccessoryButtonClicked: {
                updateWindow.visible = true;
            }
        }
    }

    onFeatureSelected: {
        // show the callout
        callout.showCallout();

        // set the combo box's default value
        damageComboBox.currentIndex = featAttributes.indexOf(updateFeaturesSample.featureType);
    }

    onHideWindow: {
        // hide the callout
        if (callout.visible)
            callout.dismiss();
        // hide the update window
        updateWindow.visible = false;
    }

    // Update Window
    Rectangle {
        id: updateWindow
        width: childrenRect.width
        height: childrenRect.height
        anchors.centerIn: parent
        radius: 10
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
        }

        GridLayout {
            columns: 2
            anchors.margins: 5

            Text {
                Layout.columnSpan: 2
                Layout.margins: 5
                text: "Update Attribute"
                font.pixelSize: 16
            }

            ComboBox {
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.columnSpan: 2
                Layout.margins: 5
                Layout.fillWidth: true
                id: damageComboBox
                model: featAttributes
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, metrics.width);
                    }
                }
                TextMetrics {
                    id: metrics
                    font: damageComboBox.font
                }
            }

            Button {
                Layout.margins: 5
                text: "Update"
                // once the update button is clicked, hide the windows, and fetch the currently selected features
                onClicked: {
                    if (callout.visible)
                        callout.dismiss();
                    updateWindow.visible = false;
                    updateFeaturesSample.updateSelectedFeature(damageComboBox.currentText)
                }
            }

            Button {
                Layout.alignment: Qt.AlignRight
                Layout.margins: 5
                text: "Cancel"
                // once the cancel button is clicked, hide the window
                onClicked: updateWindow.visible = false;
            }
        }
    }
}
