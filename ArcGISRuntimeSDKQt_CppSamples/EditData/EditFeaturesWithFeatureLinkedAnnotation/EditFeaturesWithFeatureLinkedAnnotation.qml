// [WriteFile Name=EditFeaturesWithFeatureLinkedAnnotation, Category=EditData]
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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        // Set and keep the focus on MapView to enable keyboard navigation
        onFocusChanged: focus = true;
        Component.onCompleted: {
            forceActiveFocus();
        }

        onMouseClicked: {
            if (updateWindow.visible)
                updateWindow.visible = false;
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
                text: qsTr("AD_ADDRESS:")
                Layout.margins: 5
            }

            TextField {
                id: attAddressTextField
                selectByMouse: true
            }

            Text {
                text: qsTr("ST_STR_NAM:")
                Layout.margins: 5
            }

            TextField {
                id: attStreetTextField
                selectByMouse: true
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
                        updateWindow.visible = false;
                        model.updateSelectedFeature(attAddressTextField.text, attStreetTextField.text);
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 5
                    text: qsTr("Cancel")
                    // once the cancel button is clicked, hide the window
                    onClicked: updateWindow.visible = false;
                }
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    EditFeaturesWithFeatureLinkedAnnotationSample {
        id: model
        mapView: view

        onAddressAndStreetTextChanged: {
            updateWindow.visible = true;

            attAddressTextField.text = model.addressAndStreetText[0];
            attStreetTextField.text = model.addressAndStreetText[1];
        }
    }
}
