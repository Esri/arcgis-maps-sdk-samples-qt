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

    property var currentSectionName: ""

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
        id: nearbyFeatureButtons
        anchors.right: parent.right
        padding: 10
        visible: currentSectionName === ""
        background: Rectangle {
            color: "white"
            border.color: "black"
        }
        contentItem: Column {
            id: column
            width: 200
            spacing: 10

            Text {
                text: "Current Section"
                padding: 5
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                font.pointSize: 16
            }

            RoundButton {
                id: sbutton
                width: parent.width - 10
                Text {
                    id: buttonText
                    text: sampleModel.currentSection
                    wrapMode: Text.WordWrap
                    anchors.centerIn: parent
                    width: parent.width - 5
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                onClicked: {
                    currentSectionName = buttonText.text;
                    sampleModel.getSectionInformation(currentSectionName);
                }

            }
            Text {
                text: "Nearby Points of Interest"
                padding: 5
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                font.pointSize: 16
                visible: sampleModel.nearbyPois.length > 0
            }

            Repeater {
                visible: sampleModel.nearbyPois.count > 0
                model: sampleModel.nearbyPois
                delegate: RoundButton {
                    id: button
                    width: parent.width - 10
                    Text {
                        id: pbuttonText
                        text: modelData
                        wrapMode: Text.WordWrap
                        anchors.centerIn: parent
                        width: parent.width - 5
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    onClicked: {
                        currentSectionName = pbuttonText.text;
                        sampleModel.getPoiInformation(currentSectionName);
                    }
                }
            }
        }
    }

    Pane {
        id: pane
        width: parent.width < 300 ? parent.width : 300
        height: parent.height
        visible: currentSectionName != ""
        anchors.top: parent.top
        anchors.right: parent.right
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
                    width: scrollViewComponent.width
                    text: currentSectionName
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        bold: true
                        pointSize: 16
                    }
                    wrapMode: Text.WordWrap
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: desc
                    width: scrollViewComponent.width
                    text: sampleModel.currentDescription
                    textFormat: Text.RichText
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                    wrapMode: Text.WordWrap
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Button {
                    id: closeButton
                    anchors.horizontalCenter: parent.horizontalCenter
                    Text {
                        text: "Close"
                        anchors.centerIn: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                    onClicked: {
                        currentSectionName = ""
                        sampleModel.currentDescription = "";
                        sampleModel.currentImageUrl = "";
                    }
                }
            }
        }
    }
}
