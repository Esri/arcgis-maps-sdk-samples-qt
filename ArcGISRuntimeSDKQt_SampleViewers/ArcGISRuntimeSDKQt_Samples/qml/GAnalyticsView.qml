import QtQuick 2.0
import Telemetry 0.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.0

Item {
    id: dialogComponent
    visible: GAnalytics.isVisible

    RadialGradient {
        id: overlay
        anchors.fill: parent
        opacity: 0.7
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightgrey" }
            GradientStop { position: 0.7; color: "black" }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                dialogComponent.visible = false
            }
        }
    }

    Page {
        id: dialogWindow
        anchors.centerIn: parent
        width: 275
        height: 300
        clip: true
        background: Rectangle {
            radius: 3
        }

        header: ToolBar {
            height: 42

            Label {
                id: titleText
                anchors.centerIn: parent
                text: qsTr("Telemetry Settings")
                font {
                    family: fontFamily
                    pixelSize: 18
                }
                color: "white"
            }
        }

        ColumnLayout {
            id: items
            anchors {
                fill: parent
                margins: 10
            }
            spacing: 5

            Text {
                id: msg
                Layout.fillWidth: true
                text: qsTr("This app uses telemetry to determine which samples are being used and what searches are done in app. No location or GPS data is reported using telemetry.")
                wrapMode: Text.WordWrap
                font.pixelSize: 12
            }

            CheckBox {
                id: analyticsEnabledCheckBox
                text: "Enable analytics"
                onCheckedChanged: {
                    GAnalytics.telemetryEnabled = analyticsEnabledCheckBox.checked;
                }
                Component.onCompleted: {
                    checked = GAnalytics.telemetryEnabled;
                }
            }

            Button {
                id : closeButton
                Layout.alignment: Qt.AlignCenter
                text: qsTr("Close")
                width: 110
                height: 48
                onClicked: dialogComponent.visible = false
            }
        }
    }
}

