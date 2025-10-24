// [WriteFile Name=EditGeometriesWithProgrammaticReticleTool, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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

import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Item {
    Rectangle {
        id: settings
        width: parent.width
        height: parent.height * 0.07
        color: palette.base
        RowLayout {
            anchors.fill: parent
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                opacity: reticleModel.geometryEditorStarted ? 1.0 : 0.5
                Label {
                    anchors.centerIn: parent
                    text: qsTr("CANCEL")
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: reticleModel.geometryEditorStarted
                    onClicked: reticleModel.discardEdits()
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    anchors.centerIn: parent
                    text: qsTr("SETTINGS")
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: settingsPopup.visible = true
                }
            }
            
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                opacity: reticleModel.canUndo ? 1.0 : 0.5
                Label {
                    anchors.centerIn: parent
                    text: qsTr("DONE")
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: reticleModel.canUndo
                    onClicked: reticleModel.saveEdits()
                }
            }
        }
    }

    MapView {
        id: mapView
        anchors.top: settings.bottom
        width: parent.width
        height: parent.height * 0.86
        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    Rectangle {
        width: parent.width
        height: parent.height * 0.07
        color: palette.base
        opacity: reticleModel.multifunctionButtonEnabled ? 1.0 : 0.5
        anchors {
            top: mapView.bottom
            horizontalCenter: parent.horizontalCenter
        }

        Label {
            anchors.centerIn: parent
            text: qsTr(reticleModel.multifunctionButtonText)
            font.capitalization: Font.AllUppercase
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        MouseArea {
            anchors.fill: parent
            enabled: reticleModel.multifunctionButtonEnabled
            onClicked: reticleModel.handleMultifunctionButton()
        }
    }
    
    GeometryEditorPopup {
        id: settingsPopup
        reticleModel: reticleModel
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    EditGeometriesWithProgrammaticReticleToolSample {
        id: reticleModel
        mapView: mapView
    }
}
