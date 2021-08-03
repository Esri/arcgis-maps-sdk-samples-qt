// [WriteFile Name=Geotriggers, Category=Analysis]
// [Legal]
// Copyright 2020 Esri.

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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {
    id: applicationWindowItem

    property var currentFeatureName: ""

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    GeotriggersSample {
        id: sampleModel
        mapView: view
    }

    Control {
        id: featureSelectButtonsColumn
        anchors.right: parent.right
        padding: 10

        visible: currentFeatureName === ""

        background: Rectangle {
            color: "white"
            border.color: "black"
        }

        contentItem: Column {
            id: column
            width: 200
            spacing: 10

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                padding: 5
                text: "Current Garden Section"
                color: "#3b4e1e"
                font {
                    bold: true
                    pointSize: 20
                }
            }

            RoundButton {
                id: sButton
                width: parent.width - 10
                padding: 20

                Text {
                    id: buttonText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: sampleModel.currentSection
                    wrapMode: Text.WordWrap
                    font.bold: true
                    font.pointSize: 18
                    anchors.centerIn: parent
                    width: parent.width - 5
                    color: "#f9f9f9"
                }

                background: Rectangle {
                    radius: sButton.radius
                    color: "#3B4E1E"
                }

                enabled: buttonText.text !== "N/A"

                onClicked: {
                    currentFeatureName = buttonText.text;
                    sampleModel.getFeatureInformation(currentFeatureName);
                }

            }

            Rectangle {
                id: llline
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 20
                height: 2

                color: "#000000"

                visible: poiHeader.visible
            }

            Text {
                id: poiHeader
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                padding: 5

                text: "Points of Interest"
                font {
                    bold: true
                    pointSize: 16
                }
                color: "#ac901e"

                visible: sampleModel.poisInRange.length > 0
            }

            Repeater {
                model: sampleModel.poisInRange
                delegate: RoundButton {
                    id: button
                    width: parent.width - 10
                    padding: 20

                    Text {
                        id: poiButtonText
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width - 5

                        text: modelData
                        wrapMode: Text.WordWrap
                        font.bold: true
                        color: "#f9f9f9"
                    }

                    background: Rectangle {
                        radius: sButton.radius
                        anchors.fill: parent
                        color: "#AC901E"
                    }

                    onClicked: {
                        currentFeatureName = poiButtonText.text;
                        sampleModel.getFeatureInformation(currentFeatureName);
                    }
                }
            }
        }
    }

    Pane {
        id: pane
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width < 300 ? parent.width : 300
        height: parent.height
        visible: currentFeatureName != ""
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
                    source: sampleModel.currentImageUrl
                    fillMode: Image.PreserveAspectFit
                    width: scrollViewComponent.width
                }

                Text {
                    id: sectionNameTextBox
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    width: scrollViewComponent.width

                    text: currentFeatureName
                    font {
                        bold: true
                        pointSize: 20
                    }
                    color: "#3b4e1e"
                    wrapMode: Text.WordWrap
                }

                Text {
                    id: desc
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                    width: scrollViewComponent.width

                    text: sampleModel.currentDescription
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
                        currentFeatureName = ""
                    }
                }
            }
        }
    }
}
