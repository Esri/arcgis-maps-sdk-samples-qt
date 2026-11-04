// [WriteFile Name=UpdateLabelsAndSymbolsToScaleForVisualAccessibility, Category=Accessibility]
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
import Esri.ArcGISRuntime.Toolkit
import Esri.Samples

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        // Scale labels according to the system text-size setting
        useSystemTextScale: labelScaleCheckBox.checked

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }

        Callout {
            calloutData: view.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // Instructions, legend, label scaling toggle, and current values
    Pane {
        id: panel
        anchors {
            top: parent.top
            right: parent.right
            margins: 10
        }
        width: Math.min(350, parent.width - 20)
        height: Math.min(implicitHeight, parent.height - 20)
        padding: 10

        ScrollView {
            anchors.fill: parent
            contentWidth: panel.availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            Column {
                id: contentColumn
                width: parent.width
                spacing: 8

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    text: qsTr("Change the system text size to scale feature labels and symbols, then select a feature to view its name and coordinates.")
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "darkgrey"
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    font.bold: true
                    text: qsTr("Scaling source")
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 8

                    Label {
                        id: labelsLegendIcon
                        anchors.verticalCenter: parent.verticalCenter
                        width: Math.max(symbolsLegendIcon.width, implicitWidth)
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        text: qsTr("Aa")
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        // Wrap instead of growing wider than the panel at large text sizes
                        width: Math.min(implicitWidth, contentColumn.width - labelsLegendIcon.width - parent.spacing)
                        wrapMode: Text.Wrap
                        text: qsTr("Labels: GeoView API")
                    }
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 8

                    Image {
                        id: symbolsLegendIcon
                        anchors.verticalCenter: parent.verticalCenter
                        width: model.symbolSize
                        height: width
                        fillMode: Image.PreserveAspectFit
                        source: model.symbolSwatchUrl
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        width: Math.min(implicitWidth, contentColumn.width - symbolsLegendIcon.width - parent.spacing)
                        wrapMode: Text.Wrap
                        text: qsTr("Symbols: system text size")
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "darkgrey"
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    font.bold: true
                    text: qsTr("Toggles")
                }

                // A separate label lets the check box text wrap at large text sizes
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter

                    Item {
                        anchors.verticalCenter: parent.verticalCenter
                        width: labelScaleCheckBox.width * labelScaleCheckBox.scale
                        height: labelScaleCheckBox.height * labelScaleCheckBox.scale

                        CheckBox {
                            id: labelScaleCheckBox
                            anchors.centerIn: parent
                            checked: true
                            Accessible.name: labelScaleLabel.text
                        }
                    }

                    Label {
                        id: labelScaleLabel
                        anchors.verticalCenter: parent.verticalCenter
                        width: Math.min(implicitWidth, contentColumn.width - labelScaleCheckBox.width * labelScaleCheckBox.scale)
                        wrapMode: Text.Wrap
                        text: qsTr("Apply system text size to labels")

                        TapHandler {
                            onTapped: labelScaleCheckBox.toggle()
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "darkgrey"
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    font.bold: true
                    text: qsTr("Current controls")
                }

                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    text: view.useSystemTextScale ? qsTr("Labels: <b>scaled by MapView.useSystemTextScale.</b>")
                                                  : qsTr("Labels: <b>API text scaling is disabled.</b>")
                }
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    UpdateLabelsAndSymbolsToScaleForVisualAccessibilitySample {
        id: model
        mapView: view
    }
}
