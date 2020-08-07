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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9
import Esri.ArcGISRuntime.Toolkit.Controls 100.9

Item {

    readonly property var featAttributes: ["Blocked Street or Sidewalk", "Damaged Property", "Graffiti Complaint - Public Property", "Graffiti Complaint – Private Property", "Sewer Issues", "Sidewalk and Curb Issues", "Tree Maintenance or Damage"]

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
//        objectName: "view"

        Callout {
            id: callout
            borderWidth: 1;
            borderColor: "lightgrey"
            calloutData: model.mapView.calloutData
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                updateWindow.visible = true;
                // window to edit attributes
            }
        }

        Button {
            id: creatVersion
            text: qsTr("Create Version")
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }

            onClicked: {
                if (text === qsTr("Create Version")) {
                    model.createVersion();
                    text = qsTr("Switch to Default version")
                } else if (text === qsTr("Switch to Default version")) {
                    text = qsTr("Switch to created version")
                    model.switchVersion();
                } else if (text === qsTr("Switch to created version")) {
                    text = qsTr("Switch to Default version")
                    model.switchVersion();
                }
            }
        }

        // to be removed
        Button {
            id: fetchVersions
            text: qsTr("Fetch Versions")
            anchors {
                left: parent.left
                top: creatVersion.bottom
                margins: 3
            }

            onClicked: model.fetchVersions();
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
                    //                visible: currentVersionText.text !== "" ? true : false
                    color: "white"
                }

                Text {
                    id: currentVersionText
                    Layout.alignment: Qt.AlignHCenter
                    text: model.sgdbCurrentVersion
                    color: "white"
                }
            }
        }

        Button {
            id: applyEditsBt
            text: qsTr("Apply Edits")
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: view.attributionTop
                margins: 3
            }
            enabled: false
            visible: enabled

            onClicked: {
                enabled = false;
            }
        }

    }

    // Update Window
    Rectangle {
        id: updateWindow
        width: childrenRect.width
        height: childrenRect.height
        anchors.centerIn: parent
        radius: 10
        visible: false

        GaussianBlur {
            anchors.fill: updateWindow
            source: view
            radius: 40
            samples: 20
        }

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
//                        metrics.text = model[i];
//                        modelWidth = Math.max(modelWidth, metrics.width);
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
//                    if (callout.visible)
//                        callout.dismiss();
                    updateWindow.visible = false;
                    applyEditsBt.enabled = true;
//                    updateFeaturesSample.updateSelectedFeature(damageComboBox.currentText)
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

    // Uncomment this section when running as standalone application

    AuthenticationView {
        authenticationManager: model.authManager
    }


    // Declare the C++ instance which creates the scene etc. and supply the view
    EditWithBranchVersioningSample {
        id: model
        mapView: view

        onFeatureSelected: {
            callout.showCallout();
        }
    }
}
