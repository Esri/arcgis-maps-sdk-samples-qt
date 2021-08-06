import QtQuick 2.6
import QtQuick.Controls 2.2

Pane {
    id: aboutFeaturePane

    property var featureName: ""
    property var description: ""
    property var imageSourceUrl: ""

    anchors {
        top: parent.top
        right: parent.right
    }

    width: parent.width < 300 ? parent.width : 300
    height: parent.height
    visible: false
    clip: true

    background: Rectangle {
        color: "white"
        border.color: "black"
    }

    contentItem: ScrollView {
        id: scrollViewComponent
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        Column {
            id: sectionInfoColumn
            spacing: 20

            Image {
                id: img
                source: imageSourceUrl
                fillMode: Image.PreserveAspectFit
                width: scrollViewComponent.width
            }

            Text {
                id: sectionNameTextBox
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                width: scrollViewComponent.width

                text: featureName
                font {
                    bold: true
                    pointSize: 20
                }
                color: "#3B4E1E"
                wrapMode: Text.WordWrap
            }

            Text {
                id: descriptionTextBox
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                width: scrollViewComponent.width

                text: description
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
            }

            Button {
                id: closeButton
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    anchors.centerIn: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    text: "Close"
                }

                onClicked: {
                    aboutFeaturePane.visible = false
                }
            }
        }
    }
}
