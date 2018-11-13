// [WriteFile Name=ListRelatedFeatures, Category=Features]
// [Legal]
// Copyright 2017 Esri.

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
import QtQuick.Window 2.2
import Esri.Samples 1.0

ListRelatedFeaturesSample {
    id: rootRectangle
    clip: true
    width: 800
    height: 600    

    onHideAttributeTable: attributeView.height = 0;
    onShowAttributeTable: attributeView.height = 200;

    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        // bind the insets to the attribute view so the attribution text shows when the view expands
        viewInsets.bottom: attributeView.height
    }

    Rectangle {
        id: attributeView
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 0

        // Animate the expand and collapse of the legend
        Behavior on height {
            SpringAnimation {
                spring: 3
                damping: 0.4
            }
        }

        ListView {
            anchors {
                fill: parent
                margins: 5
            }

            clip: true
            model: relatedFeaturesModel
            spacing: 5

            // Create delegate to display the attributes
            delegate: Rectangle {
                width: rootRectangle.width
                height: childrenRect.height
                color: "transparent"

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.margins: 10
                                    
                    text: displayFieldValue
                    font.pixelSize: 12
                }
            }

            // Create a section to separate features by table
            section {
                property: "serviceLayerName"
                criteria: ViewSection.FullString
                labelPositioning: ViewSection.CurrentLabelAtStart | ViewSection.InlineLabels
                delegate: Rectangle {
                    width: rootRectangle.width
                    height: childrenRect.height
                    color: "lightsteelblue"

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: section
                        font {
                            bold: true
                            pixelSize: 13
                        }
                    }
                }
            }
        }
    }
}
