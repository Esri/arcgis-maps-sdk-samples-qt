import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.15

RoundButton {
    id: undoButton
    Layout.fillWidth: true
    radius: 5

    property string buttonName: []
    property string iconPath: []
    property alias name: value

    Image {
        id: imgComponent
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -undoText.height/2
        }
        source: iconPath
        width: 20
        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: undoText
        anchors {
            top: imgComponent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        text: buttonName
        font.pixelSize: 8
    }
}
