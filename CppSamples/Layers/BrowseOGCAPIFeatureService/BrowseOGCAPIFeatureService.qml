// [WriteFile Name=BrowseOGCAPIFeatureService, Category=Layers]
// [Legal]
// Copyright 2021 Esri.
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
import Esri.Samples
import QtQuick.Layouts

Item {
    // Declare the C++ instance which creates the map etc. and supply the view
    BrowseOGCAPIFeatureServiceSample {
        id: model
        mapView: view
    }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Add the OGC feature service UI element
        Control {
            id: uiControl
            anchors {
                right: view.right
                top: view.top
                margins: 10
            }
            padding: 10
            background: Rectangle {
                color: palette.base
                border.color: "black"
                
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: mouse => mouse.accepted = true
                    onDoubleClicked: mouse => mouse.accepted = true
                    onWheel: wheel => wheel.accepted = true
                }
            }
            contentItem: GridLayout {
                columns: 2
                Label {
                    id: instructionLabel
                    text: qsTr("Load the service, then select a layer for display")
                    font.bold: true
                    verticalAlignment: "AlignVCenter"
                    horizontalAlignment: "AlignHCenter"
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
                TextField {
                    id: serviceURLBox
                    text: model.featureServiceUrl
                    Layout.fillWidth: true
                    selectByMouse: true
                }
                Button {
                    id: connectButton
                    text: qsTr("Load service")
                    enabled: !model.serviceOrFeatureLoading
                    onClicked: model.loadService(serviceURLBox.text);

                }
                ComboBox {
                    id: featureList
                    model: model.featureCollectionList
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                Button {
                    id: loadLayerButton
                    text: qsTr("Load selected layer")
                    enabled: !model.serviceOrFeatureLoading && featureList.currentIndex >= 0
                    onClicked: model.loadFeatureCollection(featureList.currentIndex);
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                }
            }
        }

        // Pop-up error message box
        Dialog {
            id: errorMessageBox
            visible: model.errorMessage === ""? false : true
            title: model.errorMessage
            standardButtons: Dialog.Ok
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
        }
    }
}
