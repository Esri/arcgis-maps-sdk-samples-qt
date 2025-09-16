// [WriteFile Name=AddEncExchangeSet, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
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
    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }

        // Add the missing Callout component
        Callout {
            calloutData: mapView.calloutData
            accessoryButtonVisible: false
            leaderPosition: Callout.LeaderPosition.Automatic
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    ConfigureElectronicNavigationalChartsSample {
        id: model
        mapView: mapView
    }

    // ENC Settings Panel - Top Right
    Rectangle {
        id: settingsPanel
        anchors {
            top: parent.top
            right: parent.right
            margins: 12
        }
        
        // Larger, more readable sizing (max 42% of window)
        width: Math.min(280, parent.width * 0.42)
        height: Math.min(settingsColumn.implicitHeight + 24, parent.height * 0.42)
        
        color: "#f5f5f5"
        radius: 8
        border {
            color: "#999999"
            width: 1
        }
        opacity: 0.95

        ScrollView {
            anchors {
                fill: parent
                margins: 12
            }
            clip: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            
            ColumnLayout {
                id: settingsColumn
                width: parent.width
                spacing: 10

                // Header
                Label {
                    Layout.fillWidth: true
                    text: qsTr("ENC Settings")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#333333"
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#cccccc"
                }

                // Color Scheme Section
                Label {
                    text: qsTr("Color Scheme")
                    font.pixelSize: 13
                    color: "#444444"
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 5

                    RadioButton {
                        text: qsTr("Day")
                        checked: model.colorScheme === "Day"
                        onClicked: model.colorScheme = "Day"
                        font.pixelSize: 11
                    }

                    RadioButton {
                        text: qsTr("Dusk")
                        checked: model.colorScheme === "Dusk"
                        onClicked: model.colorScheme = "Dusk"
                        font.pixelSize: 11
                    }

                    RadioButton {
                        text: qsTr("Night")
                        checked: model.colorScheme === "Night"
                        onClicked: model.colorScheme = "Night"
                        font.pixelSize: 11
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#e0e0e0" }

                // Area Display Section
                Label {
                    text: qsTr("Area Display")
                    font.pixelSize: 13
                    color: "#444444"
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 5

                    RadioButton {
                        text: qsTr("Plain")
                        checked: model.areaSymbolizationType === "Plain"
                        onClicked: model.areaSymbolizationType = "Plain"
                        font.pixelSize: 11
                    }

                    RadioButton {
                        text: qsTr("Symbolized")
                        checked: model.areaSymbolizationType === "Symbolized"
                        onClicked: model.areaSymbolizationType = "Symbolized"
                        font.pixelSize: 11
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#e0e0e0" }

                // Point Symbols Section
                Label {
                    text: qsTr("Point Symbols")
                    font.pixelSize: 13
                    color: "#444444"
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 5

                    RadioButton {
                        text: qsTr("Paper Chart")
                        checked: model.pointSymbolizationType === "PaperChart"
                        onClicked: model.pointSymbolizationType = "PaperChart"
                        font.pixelSize: 11
                    }

                    RadioButton {
                        text: qsTr("Simplified")
                        checked: model.pointSymbolizationType === "Simplified"
                        onClicked: model.pointSymbolizationType = "Simplified"
                        font.pixelSize: 11
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#e0e0e0" }

                // Instructions
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Tap map features for details")
                    font.pixelSize: 10
                    color: "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    Layout.topMargin: 5
                }
            }
        }
    }
}
