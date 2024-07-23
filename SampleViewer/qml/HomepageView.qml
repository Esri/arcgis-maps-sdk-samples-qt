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
import QtQuick.Shapes
import Esri.ArcGISRuntimeSamples

Rectangle {
    visible: SampleManager.currentMode === SampleManager.HomepageView

    color: "white"

    Flickable {
        anchors {
            fill: parent
            margins: 15
        }

        contentHeight: {
            featuredSamplesText.height + featuredSamplesGrid.height + 20
        }

        clip: true

        ScrollIndicator.vertical: ScrollIndicator {}

        interactive: true

        Text {
            id: featuredSamplesText
            anchors {
                left: parent.left
                right: parent.right
            }
            horizontalAlignment: Text.AlignHCenter
            text: "# Featured Samples"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
            color: "#7938b6"
        }

        GridView {
            id: featuredSamplesGrid
            property int cellsPerRow: 3 // This initial value is almost immediately overwritten when the window width is determined
            property int rowCount: 1
            property int minColumnCount: 2
            property int delegateItemSize: 175 // The width (and height) of the item container within the grid
            property int originalItemSize: 175

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
                    delegateItemSize = Math.min(originalItemSize, (parent.width/minColumnCount));
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
                    id: backgroundRectangle
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
                            Component.onCompleted: console.log("----", font.family, font.styleName, font.pointSize)
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            drawer.close();
                            // launch sample...
                            SampleManager.currentSample = sample;
                            SampleManager.currentMode = SampleManager.LiveSampleView
                        }
                        onContainsMouseChanged: {
                            if (containsMouse) {
                                backgroundRectangle.color = "#7938b6"
                                img.anchors.margins = 3

                            } else {
                                backgroundRectangle.color = "black"
                                img.anchors.margins = 1
                            }
                        }
                    }
                }
            }
        }
    }
}
