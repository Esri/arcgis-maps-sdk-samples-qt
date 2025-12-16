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
import Calcite

Rectangle {
    visible: SampleManager.currentMode === SampleManager.HomepageView

    color: Calcite.foreground1

    Flickable {
        anchors {
            fill: parent
            margins: 15
        }

        contentHeight: {
            (favoriteSamplesGrid.visible ? favoritesText.height + favoriteSamplesGrid.height + 30 : 0) +
                    featuredSamplesText.height + featuredSamplesGrid.height + 20
        }

        clip: true

        ScrollIndicator.vertical: ScrollIndicator {}

        interactive: true

        // Delegate component for sample cards
        Component {
            id: sampleCardDelegate
            Item {
                id: cell
                width: GridView.view ? GridView.view.delegateItemSize : 175
                height: GridView.view ? GridView.view.delegateItemSize : 175

                Rectangle {
                    id: paddingRect
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    width: GridView.view ? (GridView.view.cellWidth - GridView.view.delegateItemSize) / 2 : 0
                    height: 1
                    color: "#00000000"
                }

                Rectangle {
                    id: backgroundRectangle
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

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            drawer.close();
                            SampleManager.currentSample = sample;
                            SampleManager.currentMode = SampleManager.LiveSampleView
                        }
                        onContainsMouseChanged: {
                            if (containsMouse) {
                                backgroundRectangle.color = Calcite.brandHover
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

        // Favorites section - only visible when favorites exist
        Label {
            id: favoritesText
            visible: SampleManager.favoriteSamples && SampleManager.favoriteSamples.size > 0
            anchors {
                left: parent.left
                right: parent.right
            }
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("# Favorites")
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
            color: Calcite.text2
        }

        GridView {
            id: favoriteSamplesGrid
            visible: SampleManager.favoriteSamples && SampleManager.favoriteSamples.size > 0

            property int cellsPerRow: 3
            property int rowCount: 1
            property int minColumnCount: 2
            property int delegateItemSize: 175
            property int originalItemSize: 175

            anchors {
                top: favoritesText.bottom
                topMargin: visible ? 10 : 0
                left: parent.left
                right: parent.right
            }

            height: visible ? rowCount * delegateItemSize : 0
            interactive: false

            onWidthChanged: updateCellsPerRow()
            onCountChanged: updateCellsPerRow()
            onVisibleChanged: if (visible) updateCellsPerRow()

            function updateCellsPerRow() {
                if (SampleManager.favoriteSamples && SampleManager.favoriteSamples.size > 0) {
                    delegateItemSize = Math.min(originalItemSize, (parent.width / minColumnCount));
                    cellsPerRow = Math.max(1, Math.floor(favoriteSamplesGrid.width / delegateItemSize));
                    rowCount = Math.ceil(favoriteSamplesGrid.count / cellsPerRow);
                }
            }

            cellWidth: favoriteSamplesGrid.width / cellsPerRow
            cellHeight: delegateItemSize
            clip: true
            model: SampleManager.favoriteSamples
            delegate: sampleCardDelegate
        }

        Label {
            id: featuredSamplesText
            anchors {
                top: favoriteSamplesGrid.visible ? favoriteSamplesGrid.bottom : parent.top
                topMargin: favoriteSamplesGrid.visible ? 20 : 0
                left: parent.left
                right: parent.right
            }
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("# Featured Samples")
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
            color: Calcite.text2
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
            delegate: sampleCardDelegate
        }
    }
}
