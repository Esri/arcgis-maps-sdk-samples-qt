import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Popup {
    id: settingsPopup
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    x: (parent ? (parent.width - width) / 2 : 0)
    y: parent ? parent.height * 0.12 : 0
    width: parent ? Math.min(parent.width * 0.90, 360) : 360
    height: Math.min(parent ? parent.height * 0.76 : implicitHeight, settingsColumn.implicitHeight + 24)

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
            spacing: 10
            anchors {
                horizontalCenter: parent.horizontalCenter
                margins: 12
                top: parent.top
                topMargin: 12
            }

            // Header Row with title
            RowLayout {
                Layout.fillWidth: true
                spacing: 4
                Label {
                    Layout.fillWidth: true
                    text: qsTr("ENC Settings")
                    font {
                        pixelSize: 18
                        bold: true
                    }
                    horizontalAlignment: Text.AlignHCenter
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
                font {
                    pixelSize: 14
                    bold: true
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 6
                RadioButton {
                    text: qsTr("Day")
                    checked: chartsSample.colorScheme === ConfigureElectronicNavigationalChartsSample.ColorSchemeDay
                    onClicked: chartsSample.colorScheme = ConfigureElectronicNavigationalChartsSample.ColorSchemeDay
                    font.pixelSize: 12
                }
                RadioButton {
                    text: qsTr("Dusk")
                    checked: chartsSample.colorScheme === ConfigureElectronicNavigationalChartsSample.ColorSchemeDusk
                    onClicked: chartsSample.colorScheme = ConfigureElectronicNavigationalChartsSample.ColorSchemeDusk
                    font.pixelSize: 12
                }
                RadioButton {
                    text: qsTr("Night")
                    checked: chartsSample.colorScheme === ConfigureElectronicNavigationalChartsSample.ColorSchemeNight
                    onClicked: chartsSample.colorScheme = ConfigureElectronicNavigationalChartsSample.ColorSchemeNight
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
                font {
                    pixelSize: 14
                    bold: true
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 6
                RadioButton {
                    text: qsTr("Plain")
                    checked: chartsSample.areaSymbolizationType === ConfigureElectronicNavigationalChartsSample.AreaSymbolizationTypePlain
                    onClicked: chartsSample.areaSymbolizationType = ConfigureElectronicNavigationalChartsSample.AreaSymbolizationTypePlain
                    font.pixelSize: 12
                }
                RadioButton {
                    text: qsTr("Symbolized")
                    checked: chartsSample.areaSymbolizationType === ConfigureElectronicNavigationalChartsSample.AreaSymbolizationTypeSymbolized
                    onClicked: chartsSample.areaSymbolizationType = ConfigureElectronicNavigationalChartsSample.AreaSymbolizationTypeSymbolized
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
                font {
                    pixelSize: 14
                    bold: true
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 6
                RadioButton {
                    text: qsTr("Paper Chart")
                    checked: chartsSample.pointSymbolizationType === ConfigureElectronicNavigationalChartsSample.PointSymbolizationTypePaperChart
                    onClicked: chartsSample.pointSymbolizationType = ConfigureElectronicNavigationalChartsSample.PointSymbolizationTypePaperChart
                    font.pixelSize: 12
                }
                RadioButton {
                    text: qsTr("Simplified")
                    checked: chartsSample.pointSymbolizationType === ConfigureElectronicNavigationalChartsSample.PointSymbolizationTypeSimplified
                    onClicked: chartsSample.pointSymbolizationType = ConfigureElectronicNavigationalChartsSample.PointSymbolizationTypeSimplified
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
                implicitWidth: Math.max(100, contentItem.implicitWidth + 32)
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: implicitWidth
                Layout.maximumWidth: 160
                onClicked: settingsPopup.close()
            }
        }
    }
}
