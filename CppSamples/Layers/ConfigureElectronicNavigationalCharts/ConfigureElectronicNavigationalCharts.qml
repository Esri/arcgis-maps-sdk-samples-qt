// [WriteFile Name=AddEncExchangeSet, Category=Layers]
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
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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

    // Dim background when popup open
    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: settingsPopup.opened ? 0.30 : 0
        visible: settingsPopup.opened
        MouseArea {
            anchors.fill: parent
            onClicked: settingsPopup.close()
        }
    }

    // Bottom toolbar
    Rectangle {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Math.max(parent.height * 0.08, 50)
        border.width: 1

        // Settings toggle button
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            onClicked: settingsPopup.opened ? settingsPopup.close() : settingsPopup.open()
            contentItem: Text {
                text: qsTr("Display Settings")
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // Popup containing settings
    Popup {
        id: settingsPopup
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        x: (parent.width - width) / 2
        y: parent.height * 0.12
        width: Math.min(parent.width * 0.90, 360)
        height: Math.min(parent.height * 0.76, settingsColumn.implicitHeight + 24)

        contentItem: Flickable {
            id: scrollArea
            clip: true
            contentWidth: parent.width
            contentHeight: settingsColumn.implicitHeight + 24
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            interactive: contentHeight > height
            ScrollBar.vertical: ScrollBar {
                policy: scrollArea.contentHeight > scrollArea.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            }

            ColumnLayout {
                id: settingsColumn
                width: scrollArea.width - 24
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10
                anchors.margins: 12
                anchors.top: parent.top
                anchors.topMargin: 12

                // Header Row with title
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("ENC Settings")
                        font.pixelSize: 18
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#cccccc"
                }

                // Color Scheme Section
                Label {
                    text: qsTr("Color Scheme")
                    font.pixelSize: 14
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 6
                    RadioButton {
                        text: qsTr("Day")
                        checked: model.colorScheme === ConfigureElectronicNavigationalChartsSample.EncColorSchemeDay
                        onClicked: model.colorScheme = ConfigureElectronicNavigationalChartsSample.EncColorSchemeDay
                        font.pixelSize: 12
                    }
                    RadioButton {
                        text: qsTr("Dusk")
                        checked: model.colorScheme === ConfigureElectronicNavigationalChartsSample.EncColorSchemeDusk
                        onClicked: model.colorScheme = ConfigureElectronicNavigationalChartsSample.EncColorSchemeDusk
                        font.pixelSize: 12
                    }
                    RadioButton {
                        text: qsTr("Night")
                        checked: model.colorScheme === ConfigureElectronicNavigationalChartsSample.EncColorSchemeNight
                        onClicked: model.colorScheme = ConfigureElectronicNavigationalChartsSample.EncColorSchemeNight
                        font.pixelSize: 12
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#e0e0e0"
                }

                // Area Display Section
                Label {
                    text: qsTr("Area Display")
                    font.pixelSize: 14
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 6
                    RadioButton {
                        text: qsTr("Plain")
                        checked: model.areaSymbolizationType === ConfigureElectronicNavigationalChartsSample.EncAreaSymbolizationTypePlain
                        onClicked: model.areaSymbolizationType = ConfigureElectronicNavigationalChartsSample.EncAreaSymbolizationTypePlain
                        font.pixelSize: 12
                    }
                    RadioButton {
                        text: qsTr("Symbolized")
                        checked: model.areaSymbolizationType === ConfigureElectronicNavigationalChartsSample.EncAreaSymbolizationTypeSymbolized
                        onClicked: model.areaSymbolizationType = ConfigureElectronicNavigationalChartsSample.EncAreaSymbolizationTypeSymbolized
                        font.pixelSize: 12
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#e0e0e0"
                }

                // Point Symbols Section
                Label {
                    text: qsTr("Point Symbols")
                    font.pixelSize: 14
                    font.bold: true
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 6
                    RadioButton {
                        text: qsTr("Paper Chart")
                        checked: model.pointSymbolizationType === ConfigureElectronicNavigationalChartsSample.EncPointSymbolizationTypePaperChart
                        onClicked: model.pointSymbolizationType = ConfigureElectronicNavigationalChartsSample.EncPointSymbolizationTypePaperChart
                        font.pixelSize: 12
                    }
                    RadioButton {
                        text: qsTr("Simplified")
                        checked: model.pointSymbolizationType === ConfigureElectronicNavigationalChartsSample.EncPointSymbolizationTypeSimplified
                        onClicked: model.pointSymbolizationType = ConfigureElectronicNavigationalChartsSample.EncPointSymbolizationTypeSimplified
                        font.pixelSize: 12
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#e0e0e0"
                }

                // Footer close button
                Button {
                    text: qsTr("Close")
                    Layout.alignment: Qt.AlignHCenter
                    implicitWidth: Math.max(100, contentItem.implicitWidth + 32)
                    Layout.preferredWidth: implicitWidth
                    Layout.maximumWidth: 160
                    onClicked: settingsPopup.close()
                }
            }
        }
    }
}
