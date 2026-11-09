// [WriteFile Name=GetFieldFunctionValueFromAPoint, Category=Analysis]
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
import Esri.ArcGISRuntime.Toolkit

Item {
    id: root

    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
        focus: true

        Callout {
            id: callout
            calloutData: mapView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
            maxWidth: Math.max(0, Math.min(300 * font.pixelSize / 16, mapView.width - leftPadding - rightPadding - 24))
            contentWidth: Math.min(maxWidth, contentItem.implicitWidth)

            contentItem: Label {
                text: callout.calloutData ? callout.calloutData.detail : ""
                textFormat: Text.PlainText
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    Frame {
        id: settingsPanel
        anchors {
            top: parent.top
            right: parent.right
            margins: 12
        }
        width: Math.min(320, Math.max(0, root.width - 24))
        height: Math.min(settingsColumn.implicitHeight + topPadding + bottomPadding, Math.max(0, root.height * 0.5 - 24))
        padding: 12

        MouseArea {
            x: -settingsPanel.leftPadding
            y: -settingsPanel.topPadding
            width: settingsPanel.width
            height: settingsPanel.height
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: mouse => mouse.accepted = true
            onDoubleClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }

        ScrollView {
            id: settingsScroll
            anchors.fill: parent
            clip: true
            contentWidth: width
            contentHeight: settingsColumn.implicitHeight
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                id: settingsColumn
                width: settingsScroll.width
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        id: settingsButton
                        Layout.minimumHeight: 44
                        text: qsTr("Settings")
                        checkable: true
                        enabled: model.initialized
                    }

                    BusyIndicator {
                        Layout.preferredWidth: 24
                        Layout.preferredHeight: 24
                        running: visible
                        visible: model.loading || model.analysisUpdating
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    active: settingsButton.checked
                    visible: active
                    sourceComponent: ColumnLayout {
                        spacing: 8

                        Label {
                            Layout.fillWidth: true
                            text: qsTr("Aircraft altitude (MSL): %1 m").arg(Number(model.aircraftAltitude).toLocaleString(Qt.locale(), "f", 0))
                            wrapMode: Text.WordWrap
                            font.bold: true
                        }

                        Slider {
                            Layout.fillWidth: true
                            Layout.minimumHeight: 44
                            from: 1500
                            to: 2595
                            stepSize: (to - from) / 100
                            snapMode: Slider.SnapAlways
                            value: model.aircraftAltitude
                            onMoved: model.aircraftAltitude = value
                        }

                        Label {
                            Layout.fillWidth: true
                            text: qsTr("Red areas have no ground clearance at the selected altitude.")
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }
    }

    GetFieldFunctionValueFromAPointSample {
        id: model
        mapView: mapView
    }
}
