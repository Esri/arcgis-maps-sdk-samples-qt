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
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples
import Esri.ArcGISRuntime.Toolkit

Item {

    readonly property var featAttributes: ["Affected", "Destroyed", "Inaccessible", "Minor", "Major"]
    readonly property var versionAccessModel: ["Public", "Protected", "Private"]

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            id: callout
            background: Rectangle {
                border.color: "lightgrey"
                border.width: 1    
            }
            calloutData: model.mapView.calloutData
            leaderPosition: Callout.LeaderPosition.Automatic
            onAccessoryButtonClicked: {
                for (let i=0; i < featAttributes.length; i++) {
                    if (model.currentTypeDamage === featAttributes[i]) {
                        typeDmgCombo.currentIndex = i;
                        break;
                    }
                }
                updateWindow.visible = true;
            }
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
                    model.applyEdits();
                    callout.dismiss();
                    updateWindow.visible = false;
                } else if (text === qsTr("Switch to created version")) {
                    text = qsTr("Switch to default version")
                    model.switchVersion();
                    callout.dismiss();
                    updateWindow.visible = false;
                }
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
                    text: model.sgdbCurrentVersion
                    color: "white"
                }
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
            onClicked: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
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
                        model.createVersion(versionNameTextField.text, accessComboBox.currentValue, descriptionTextField.text);
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
            onClicked: mouse => mouse.accepted = true;
            onWheel: wheel => wheel.accepted = true;
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
<<<<<<< HEAD
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding
=======
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + 20
>>>>>>> v.next
                Layout.margins: 5
                Layout.fillWidth: true
                enabled: !model.sgdbVersionIsDefault
                model: featAttributes
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
                    enabled: !model.sgdbVersionIsDefault
                    // once the update button is clicked, hide the windows, and fetch the currently selected features
                    onClicked: {
                        print("Update Attriubte " + typeDmgCombo.currentValue);
                        model.updateAttribute(typeDmgCombo.currentValue);
                        updateWindow.visible = false;
                        callout.dismiss();
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    // once the cancel button is clicked, hide the window
                    onClicked: {
                        model.clearSelectedFeature();
                        updateWindow.visible = false;
                        callout.dismiss();
                    }
                }
            }
        }
    }

    function resetCreateVersionWindow() {
        createVersionWindow.visible = false;
        versionNameTextField.text = "";
        descriptionTextField.text = "";
        accessComboBox.currentIndex = 0;
    }

    // Declare AuthenticationView to handle any authentication challenges
    AuthenticationView {
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    EditWithBranchVersioningSample {
        id: model
        mapView: view

        onFeatureSelected: {
            callout.showCallout();
        }

        onHideWindow: {
            // hide the callout
            if (callout.visible)
                callout.dismiss();
            // hide the update window
            updateWindow.visible = false;
        }

        onCreateVersionSuccess: {
            createVersionBtn.text = qsTr("Switch to default version");
        }

        onErrorMessageChanged: {
            errorDialog.visible = true;
        }

        onApplyingEdits: {
            applyEditsDialog.visible = true;
        }

        onApplyingEditsCompleted: {
            applyEditsDialog.visible = false;
        }
    }

    Dialog {
        id: errorDialog
        anchors.centerIn: parent
        standardButtons: Dialog.Ok

        Text {
            id: errorText
            text: model.errorMessage;
        }
    }

    Dialog {
        id: applyEditsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) - view.attributionRect.height;

        Text {
            id: applyEditsText
            text: qsTr("Applying Edits")
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: model.busy
    }
}
