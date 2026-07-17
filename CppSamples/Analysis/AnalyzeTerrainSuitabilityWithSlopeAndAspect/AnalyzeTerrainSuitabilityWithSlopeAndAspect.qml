// [WriteFile Name=AnalyzeTerrainSuitabilityWithSlopeAndAspect, Category=Analysis]
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
    readonly property bool compactUi: width < 760

    AnalyzeTerrainSuitabilityWithSlopeAndAspectSample {
        id: model
    }

    MapView {
        id: view
        anchors.fill: parent
        objectName: "mapView"
        focus: true

        Component.onCompleted: forceActiveFocus()
    }

    Component.onCompleted: {
        model.mapView = view
    }

    Rectangle {
        id: settingsPanel
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: compactUi ? 26 : 32
        anchors.rightMargin: compactUi ? 10 : 16
        radius: 12
        color: palette.base
        border.color: "black"
        border.width: 2
        opacity: 0.85
        z: 5
        readonly property int panelPadding: compactUi ? 10 : 14
        width: Math.min(settingsColumn.implicitWidth + panelPadding * 2, parent.width - anchors.rightMargin * 2)
        height: settingsColumn.implicitHeight + panelPadding * 2

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ColumnLayout {
            id: settingsColumn
            anchors.fill: parent
            anchors.margins: settingsPanel.panelPadding
            spacing: compactUi ? 4 : 6

            Label {
                text: qsTr("Sheltered vs Exposed Terrain Suitability")
                font.bold: true
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.preferredWidth: 280
            }

            Label {
                text: qsTr("Choose a preconfigured terrain suitability scenario.")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                Layout.preferredWidth: 280
            }

            ButtonGroup {
                id: scenarioGroup
            }

            RadioButton {
                ButtonGroup.group: scenarioGroup
                checked: model.selectedScenario === AnalyzeTerrainSuitabilityWithSlopeAndAspectSample.GentleSouthFacingSlopes
                text: qsTr("Gentle, lowland south-facing slopes")
                Layout.fillWidth: true
                onClicked: model.selectedScenario = AnalyzeTerrainSuitabilityWithSlopeAndAspectSample.GentleSouthFacingSlopes
            }

            RadioButton {
                ButtonGroup.group: scenarioGroup
                checked: model.selectedScenario === AnalyzeTerrainSuitabilityWithSlopeAndAspectSample.SteepWestAndNorthFacingSlopes
                text: qsTr("Steep, upland west- through north-facing slopes")
                Layout.fillWidth: true
                onClicked: model.selectedScenario = AnalyzeTerrainSuitabilityWithSlopeAndAspectSample.SteepWestAndNorthFacingSlopes
            }
        }
    }

    Label {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 8
        anchors.topMargin: 8
        text: qsTr("Raster data Copyright Scottish Government and SEPA (2014)")
        font.italic: true
        font.pointSize: 12
        color: "black"
        font.bold: true
    }
}
