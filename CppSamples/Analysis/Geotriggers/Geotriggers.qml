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
import QtQuick.Layouts
import Esri.Samples

Item {
    id: applicationWindowItem

    property var currentFeatureName: ""

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    GeotriggersSample {
        id: sampleModel
        mapView: view
    }

    // The featureSelectButtonsColumn displays the current section as well as points of interest within 10 meters
    // Buttons are added and removed when a feature fence has been entered or exited.
    Control {
        id: featureSelectButtonsColumn
        anchors.right: parent.right
        padding: 10

        visible: !featureInfoPane.visible

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
                color: "#3B4E1E"
                font {
                    bold: true
                    pointSize: 20
                }
            }

            RoundButton {
                id: sectionButton
                width: parent.width - 10
                padding: 20

                Text {
                    id: buttonText
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    width: parent.width - 5

                    text: sampleModel.currentSection
                    font {
                        bold: true
                        pointSize: 18
                    }
                    wrapMode: Text.WordWrap
                    color: "white"
                }

                background: Rectangle {
                    radius: sectionButton.radius
                    color: "#3B4E1E"
                }

                enabled: buttonText.text !== "N/A"

                onClicked: {
                    currentFeatureName = buttonText.text;
                    sampleModel.getFeatureInformation(currentFeatureName);
                    featureInfoPane.visible = true
                }
            }

            Rectangle {
                id: dividingLine
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 20
                height: 2

                color: "black"

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
                color: "#AC901E"

                visible: sampleModel.poisInRange.length > 0
            }

            Repeater {
                model: sampleModel.poisInRange
                delegate: RoundButton {
                    id: poiButton
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
                        color: "white"
                    }

                    background: Rectangle {
                        anchors.fill: parent
                        radius: sectionButton.radius
                        color: "#AC901E"
                    }

                    onClicked: {
                        currentFeatureName = poiButtonText.text;
                        sampleModel.getFeatureInformation(currentFeatureName);
                        featureInfoPane.visible = true
                    }
                }
            }
        }
    }

    // The FeatureInfoPane displays the name, description, and image retrieved from a fence feature.
    FeatureInfoPane {
        id: featureInfoPane

        featureName: currentFeatureName
        description: sampleModel.currentDescription
        imageSourceUrl: sampleModel.currentImageUrl
    }
}
