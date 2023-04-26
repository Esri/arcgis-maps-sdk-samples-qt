// [Legal]
// Copyright 2023 Esri.

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
import Esri.ArcGISRuntimeSamples
import Telemetry

Rectangle {
    visible: SampleManager.currentMode === SampleManager.HomepageView

    Flickable {
        anchors {
            fill: parent
            margins: 15
        }

        contentHeight: {
            titleText.height + descText.height + hrLine.height + featuredSamplesText.height + featuredSamplesGrid.height + 40
        }

        clip: true

        ScrollIndicator.vertical: ScrollIndicator {}

        interactive: true

        Text {
            id: titleText
            anchors {
                left: parent.left
                right: parent.right
                margins: 0
            }

            text: "# ArcGIS Maps SDK for Qt Sample Viewer"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
            color: "#7938b6"

        }
        Text {
            id: descText
            anchors {
                top: titleText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
            }

            text: "
The ArcGIS Maps SDK for Qt Sample Viewer presents an overview of what you can achieve using the ArcGIS Maps SDK for Qt in your Qt applications.
This application contains several samples that demonstrate the power and ease-of-use of the ArcGIS Maps SDK for Qt product, and more samples are added with each release.
You can browse all the samples by navigating through the different categories.
"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText

        }

        Rectangle {
            id: hrLine
            anchors {
                top: descText.bottom
                margins: 10
            }

            width: parent.width
            height: 1
            color: "#000000"
        }

        Text {
            id: featuredSamplesText
            anchors {
                top: hrLine.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
            }
            text: "### Featured Samples"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
        }

        GridView {
            id: featuredSamplesGrid
            property int cellsPerRow: 3 // This initial value is almost immediately overwritten when the window width is determined
            property int rowCount: 1
            property int delegateItemSize: 175 // The width (and height) of the item container within the grid

            anchors {
                top: featuredSamplesText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
            }

            height: rowCount * delegateItemSize

            interactive: false

            onWidthChanged: {
                updateCellsPerRow();
            }

            onCountChanged: {
                updateCellsPerRow();
            }

            function updateCellsPerRow() {
                if (SampleManager.featuredSamples) {
                    cellsPerRow = Math.min(Math.floor(featuredSamplesGrid.width/delegateItemSize), SampleManager.featuredSamples.size);
                    rowCount = Math.ceil(featuredSamplesGrid.count / cellsPerRow);
                }
            }

            cellWidth: featuredSamplesGrid.width/cellsPerRow
            cellHeight: delegateItemSize
            clip: true
            model: SampleManager.featuredSamples
            delegate: Item {
                id: cell
                width: featuredSamplesGrid.delegateItemSize
                height: featuredSamplesGrid.delegateItemSize

                Rectangle {
                    // This transparent rectangle is included to ensure items are spread out evenly and are repositioned as the window resizes
                    id: paddingRect
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: (featuredSamplesGrid.cellWidth - featuredSamplesGrid.delegateItemSize)/2
                    height: 1
                    color: "#00000000"
                }

                Rectangle {
                    // This rectangle acts as a border around each grid item
                    anchors {
                        top: parent.top
                        left: paddingRect.right
                    }
                    height: cell.height * .90
                    width: cell.width * .90
                    color: "black"

                    Image {
                        id: img

                        anchors.fill: parent
                        anchors.margins: 1
                        source: thumbnailUrl
                        fillMode: Image.PreserveAspectCrop
                    }

                    Rectangle {
                        id: txtBackground
                        anchors {
                            bottom: img.bottom
                            topMargin: cell.height * .05
                            left: img.left
                            right: img.right
                        }
                        height: txt.height
                        color: "#80000000"

                        Text {
                            id: txt
                            padding: 5
                            width: txtBackground.width - (padding * 2)
                            text: name
                            wrapMode: Text.WordWrap
                            font.bold: true
                            clip: true

                            color: "white"

                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawer.close();
                        // launch sample...
                        SampleManager.currentSample = sample;
                        GAnalytics.postEvent("sample_opened", {"sample_name": sample.name, "referrer": "featured list"});
                        SampleManager.currentMode = SampleManager.LiveSampleView
                    }
                }
            }
        }
    }
}
