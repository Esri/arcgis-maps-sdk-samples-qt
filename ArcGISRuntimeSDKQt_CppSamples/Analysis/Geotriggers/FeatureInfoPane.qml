// [WriteFile Name=Geotriggers, Category=Analysis]
// [Legal]
// Copyright 2021 Esri.

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
        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

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
