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

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.4

UpdateAttributesFeatureServiceSample {
    id: updateFeaturesSample
    width: 800
    height: 600

    property real scaleFactor: (Screen.logicalPixelDensity * 25.4) / (Qt.platform.os === "windows" || Qt.platform.os === "linux" ? 96 : 72)
    property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            borderWidth: 1 * scaleFactor;
            borderColor: "lightgrey"
            calloutData: updateFeaturesSample.calloutData
            leaderPosition: leaderPositionEnum.Automatic
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
        anchors.centerIn: parent
        width: 200 * scaleFactor
        height: 110 * scaleFactor
        radius: 10 * scaleFactor
        visible: false

        GaussianBlur {
            anchors.fill: updateWindow
            source: mapView
            radius: 40
            samples: 20
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true;
            onWheel: wheel.accepted = true;
        }

        Column {
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            spacing: 10 * scaleFactor
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    text: "Update Attribute"
                    font.pixelSize: 16 * scaleFactor
                }
            }

            ComboBox {
                id: damageComboBox
                width: updateWindow.width - (20 * scaleFactor)
                model: featAttributes
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10 * scaleFactor

                Button {
                    width: (updateWindow.width / 2) - (20 * scaleFactor)
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
                    width: (updateWindow.width / 2) - (20 * scaleFactor)
                    text: "Cancel"
                    // once the cancel button is clicked, hide the window
                    onClicked: updateWindow.visible = false;
                }
            }
        }
    }
}
