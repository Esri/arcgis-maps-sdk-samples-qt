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

    readonly property var featAttributes: ["Affected", "Destroyed", "Inaccessible", "Minor", "Major"]
    readonly property var versionAccessModel: ["Public", "Protected", "Private"]

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
                for (let i=0; i < featAttributes.length; i++) {
                    if (model.currentTypeDamage === featAttributes[i]) {
                        typeDmgCombo.currentIndex = i;
                        updateWindow.visible = true;
                        return;
                    }
                }


                // window to edit attributes
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
//                    text = qsTr("Switch to Default version")
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
//            visible: false
            anchors {
                left: parent.left
                top: createVersionBtn.bottom
                margins: 3
            }

            onClicked: model.fetchVersions();
        }

        Button {
            id: switchVersions
            text: qsTr("Switch Versions")
//            visible: false
            anchors {
                left: parent.left
                top: fetchVersions.bottom
                margins: 3
            }

            onClicked: model.switchVersion2();
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

                Text {
                    text: qsTr("Version access:")
                    Layout.alignment: Qt.AlignHCenter
                    //                visible: currentVersionText.text !== "" ? true : false
                    color: "white"
                }

                Text {
                    id: versionAccessText
                    Layout.alignment: Qt.AlignHCenter
                    text: model.sgdbVersionAccess
                    color: "white"
                }

                Text {
                    text: qsTr("description:")
                    Layout.alignment: Qt.AlignHCenter
                    //                visible: currentVersionText.text !== "" ? true : false
                    color: "white"
                }

                Text {
                    id: descriptionText
                    Layout.alignment: Qt.AlignHCenter
                    text: model.sgdbVersionDescription
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

        GaussianBlur {
            anchors.fill: createVersionWindow
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

            TextField {
                id: versionNameTextField
                placeholderText: qsTr("Name must be unique")
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
                validator: RegExpValidator { regExp: /\w{0,50}/ }
            }

            ComboBox {
                id: accessComboBox
                model: versionAccessModel
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
            }

            TextField {
                id: descriptionTextField
                placeholderText: qsTr("Enter description")
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter
                Layout.margins: 5
            }

            Row {
                Layout.alignment: Qt.AlignRight
                Layout.columnSpan: 2
                Layout.margins: 5
                height: childrenRect.height
                spacing: 5

                Button {
                    Layout.margins: 5
                    Layout.alignment: Qt.AlignRight
                    text: qsTr("Create")
                    // once the update button is clicked, hide the windows, and fetch the currently selected features
                    onClicked: {
                        model.createVersion(versionNameTextField.text, accessComboBox.currentValue, descriptionTextField.text);
                        resetCreateVersionWindow();
//                        createVersionBtn.text = qsTr("Switch to Default version");
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    // once the cancel button is clicked, hide the window
                    onClicked: {
                        resetCreateVersionWindow();
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

    // Update Window
    Rectangle {
        id: updateWindow
        anchors.centerIn: parent
        width: childrenRect.width
        height: childrenRect.height
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

                        updateWindow.visible = false;
                        callout.dismiss();
                    }
                }
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

        onHideWindow: {
            // hide the callout
            if (callout.visible)
                callout.dismiss();
            // hide the update window
            updateWindow.visible = false;
        }

        onCreateVersionSuccess: {
            createVersionBtn.text = qsTr("Switch to Default version");
        }

        onErrorMessageChanged: {
            errorDialog.visible = true;
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

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
//        visible: model.busy
    }
}
