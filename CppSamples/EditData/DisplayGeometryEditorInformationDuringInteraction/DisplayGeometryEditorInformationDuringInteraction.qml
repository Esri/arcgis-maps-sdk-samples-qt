// [WriteFile Name=DisplayGeometryEditorInformationDuringInteraction, Category=EditData]
// [Legal]
// Copyright 2026 Esri.
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

Item {
    id: root

    MapView {
        id: mapView
        anchors.fill: root

        Component.onCompleted: forceActiveFocus()
    }

    DisplayGeometryEditorInformationDuringInteractionSample {
        id: sampleModel
        mapView: mapView
    }

    Control {
        width: Math.max(0, Math.min(380, root.width - 20))
        padding: 10

        anchors {
            top: root.top
            right: root.right
            margins: 10
        }

        background: Rectangle {
            color: palette.base
            opacity: 0.9

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: mouse => mouse.accepted = true
                onDoubleClicked: mouse => mouse.accepted = true
                onWheel: wheel => wheel.accepted = true
            }
        }

        contentItem: ColumnLayout {
            spacing: 8

            Label {
                visible: !sampleModel.geometryEditorStarted
                text: qsTr("Tap a graphic to start the geometry editor.")
                wrapMode: Text.WordWrap

                Layout.fillWidth: true
            }

            GridLayout {
                columns: 2
                visible: sampleModel.geometryEditorStarted

                Button {
                    text: qsTr("Done")

                    Layout.fillWidth: true

                    onClicked: sampleModel.saveEdits()
                }

                Button {
                    text: qsTr("Cancel")

                    Layout.fillWidth: true

                    onClicked: sampleModel.cancelEdits()
                }

                Button {
                    text: qsTr("Undo")
                    enabled: sampleModel.canUndo

                    Layout.fillWidth: true

                    onClicked: sampleModel.undo()
                }

                Button {
                    text: qsTr("Redo")
                    enabled: sampleModel.canRedo

                    Layout.fillWidth: true

                    onClicked: sampleModel.redo()
                }
            }

            RowLayout {
                visible: sampleModel.interactionInfoVisible

                Layout.fillWidth: true

                Label {
                    text: sampleModel.interactionDescription
                    wrapMode: Text.WordWrap
                }

                Label {
                    text: sampleModel.interactionValue
                    // ensure the size of the text displayed doesn't
                    // judder as the numbers rapidly change on interacting
                    font.features: ({ "tnum": 1 })
                }
            }
        }
    }
}
