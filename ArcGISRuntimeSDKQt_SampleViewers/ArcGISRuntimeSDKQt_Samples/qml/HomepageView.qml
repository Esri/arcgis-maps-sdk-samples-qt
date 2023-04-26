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
        id: flickable
        anchors {
            margins: 15
            fill: parent
        }

        ScrollIndicator.vertical: ScrollIndicator {}

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
                margins: 5
            }

            width: parent.width
            height: 1
            color: "#000000"
        }

        Text {
            id: featuredSamplesText
            anchors {
                top: hrLine.bottom
                topMargin: 5
                left: parent.left
                right: parent.right
            }
            text: "### Featured Samples"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
        }

        GridView {
            id: featuredSamplesGrid
            property int cellsPerRow: 3
            property int delegateItemWidth: 175
            anchors {
                top: featuredSamplesText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            onWidthChanged: {
                updateCellsPerRow();
            }

            onCountChanged: {
                updateCellsPerRow();
            }

            function updateCellsPerRow() {
                if (SampleManager.featuredSamples)
                    cellsPerRow = Math.min(Math.floor(featuredSamplesGrid.width/delegateItemWidth), SampleManager.featuredSamples.size);
            }

            cellWidth: (featuredSamplesGrid.width/cellsPerRow)
            cellHeight: delegateItemWidth
            clip: true
            model: SampleManager.featuredSamples
            delegate: Item {
                id: cell
                width: featuredSamplesGrid.delegateItemWidth
                height: featuredSamplesGrid.cellHeight

                Rectangle {
                    id: paddingRect
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: (featuredSamplesGrid.cellWidth - featuredSamplesGrid.delegateItemWidth)/2
                    height: 1
                    color: "#00000000"
                }

                Image {
                    id: img
                    anchors {
                        top: parent.top
                        topMargin: cell.height * .05
                        left: paddingRect.right
                    }

                    height: cell.height * .60
                    width: cell.width * .90
                    source: thumbnailUrl
                    fillMode: Image.PreserveAspectCrop
                }

                Text {
                    id: txt
                    anchors {
                        top: img.bottom
                        topMargin: cell.height * .05
                        left: img.left
                        right: img.right
                    }

                    height: cell.height * .30

                    horizontalAlignment: Text.AlignHCenter
                    text: name
                    wrapMode: Text.WordWrap
                    font.bold: true
                    clip: true
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
