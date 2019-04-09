import QtQuick.Controls 2.2
import QtQuick 2.6

Slider {
    id: slider
    property alias text: handleText.text
    property alias handleWidth: handleText.width
    font.pixelSize: 14
    handle: Item {
        x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - handleNub.width)
        y: parent.topPadding + parent.availableHeight / 2 - handleNub.height / 2
        Rectangle {
            id: handleNub
            color: handleRect.color
            radius: width * 0.5
            width: 10
            height: width
        }
        Rectangle {
            id: handleRect
            height: childrenRect.height
            width: childrenRect.width
            radius: 3
            x: handleNub.x - width / 2 + handleNub.width / 2
            y: handleNub.y - handleNub.height * 2
            color: progressSlider.background.children[0].color
            Text {
                id: handleText
                padding: 3
                color: "white"
                font: slider.font
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }
}
