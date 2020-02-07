// [WriteFile Name=DisplaySubtypeFeatureLayer, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: controlsBox
            anchors {
                left: parent.left
                top: parent.top
                margins: 3
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"
            opacity: 0.8
            radius: 5

            // catch mouse signals from propagating to parent
            MouseArea {
                anchors.fill: parent
                onClicked: mouse.accepted = true
                onWheel: wheel.accepted = true
            }

            ColumnLayout {
                id: controlItemsLayout

                CheckBox {
                    text: qsTr("Show sublayer")
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                    checked: true
                    enabled: !busy.visible
                    onCheckedChanged: model.switchSublayerVisibility();
                }

                RadioButton {
                    text: qsTr("Show original renderer")
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                    checked: true
                    enabled: !busy.visible
                    onCheckedChanged: {
                        if (checked)
                            model.setOringalRenderer();
                    }
                }

                RadioButton {
                    text: qsTr("Show alternative renderer")
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                    enabled: !busy.visible
                    onCheckedChanged: {
                        if (checked)
                            model.setAlternativeRenderer();
                    }
                }

                Shape {
                    id: pageBreak
                    height: 2
                    ShapePath {
                        strokeWidth: 1
                        strokeColor: "black"
                        strokeStyle: ShapePath.SolidLine
                        startX: 2; startY: 0
                        PathLine { x: controlItemsLayout.width - 2 ; y: 0 }
                    }
                }

                Text {
                    text: qsTr("Current map scale: 1:%1".arg(Math.round(model.mapScale)))
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                }

                Text {
                    text: qsTr("Sublayer min scale: %1".arg(model.sublayerMinScale > 0.0 ? Math.round(model.sublayerMinScale) : "not set"))
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                }

                Button {
                    text: qsTr("Set sublayer minimum scale")
                    Layout.margins: 2
                    Layout.alignment: Qt.AlignLeft
                    enabled: !busy.visible
                    onClicked: model.setSublayerMinScale();
                }
            }
        }

        BusyIndicator {
            id: busy
            anchors.centerIn: parent
            visible: model.busy
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplaySubtypeFeatureLayerSample {
        id: model
        mapView: view
    }
}
