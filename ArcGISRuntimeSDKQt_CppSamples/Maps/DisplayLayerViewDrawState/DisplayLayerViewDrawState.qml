// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
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
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Dialogs 100.9

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Dialog {
            id: warningDialog
            anchors.centerIn: parent
            standardButtons: Dialog.Ok
            visible: model.warningMessage !== "" ? true : false
            Text {
                text: model.warningMessage;
            }
        }

        Rectangle {
            id: controlRect
            anchors {
                bottom: loadLayerButton.top
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "#1976D2"
            radius: 3

            MouseArea {
                width: controlLayout.childrenRect.width
                height: controlLayout.childrenRect.height
                onClicked: mouse.accepted = true;
                onWheel: wheel.accepted = true;
            }

            ColumnLayout{
                id: controlLayout

                Text {
                    id: textHeader
                    text: qsTr("Current view status:")
                    color: "white"
                }

                Column {
                    id: column
                    Layout.alignment: Qt.AlignHCenter
                    Repeater {
                        id: layerViewStatusRepeater
                        model: model.viewStatus
                        Layout.alignment: Qt.AlignHCenter
                        Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            Text {
                                text: modelData
                                color: "white"
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: loadLayerButton
            anchors {
                bottom: view.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            text: qsTr("Load Layer")
            enabled: !model.loading
            clip: true
            onClicked: {
                if (text === qsTr("Load Layer")) {
                    model.loadLayer();
                    text = qsTr("Hide Layer");
                } else if (text === qsTr("Hide Layer")) {
                    text = qsTr("Show Layer");
                    model.changeFeatureLayerVisibility(false);

                } else if (text === qsTr("Show Layer")) {
                    text = qsTr("Hide Layer");
                    model.changeFeatureLayerVisibility(true);
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplayLayerViewDrawStateSample {
        id: model
        mapView: view
    }
}
