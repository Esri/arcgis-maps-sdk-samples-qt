// [WriteFile Name=ShowLineOfSightAnalysisInMap, Category=Analysis]
// [Legal]
// Copyright 2026 Esri.

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
    id: root
    width: 800
    height: 600
    clip: true

    ShowLineOfSightAnalysisInMapSample {
        id: model
        mapView: view
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MapView {
            id: view
            objectName: "mapView"
            Layout.fillWidth: true
            Layout.fillHeight: true

            Component.onCompleted: {
                forceActiveFocus();
            }

            Callout {
                id: callout
                calloutData: view.calloutData
                maxWidth: Math.min(root.width * 0.6, 320)
                implicitHeight: 120
                screenOffsetY: -19
                accessoryButtonVisible: false
                leaderPosition: Callout.LeaderPosition.Automatic
            }

            Label {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 12
                color: palette.text
                font.pixelSize: 12
                font.italic: true
                font.bold: true
                text: qsTr("Raster data Copyright Scottish Government and SEPA (2014)")
            }

            Rectangle {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 12
                width: filterRow.implicitWidth + 16
                height: filterRow.implicitHeight + 16
                radius: 6
                color: palette.base
                opacity: 0.85
                border.color: palette.mid

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: mouse => mouse.accepted = true
                    onDoubleClicked: mouse => mouse.accepted = true
                    onWheel: wheel => wheel.accepted = true
                }

                RowLayout {
                    id: filterRow
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    Label {
                        color: palette.text
                        text: qsTr("Only observers with line of sight")
                    }

                    Switch {
                        checked: model.visibilityFilter
                        onToggled: model.setVisibilityFilter(checked)
                    }
                }
            }
        }

    }
}
