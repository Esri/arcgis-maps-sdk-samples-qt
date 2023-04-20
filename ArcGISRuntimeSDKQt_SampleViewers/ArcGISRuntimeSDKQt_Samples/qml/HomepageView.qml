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

        //contentHeight: textEdit.height
        ScrollIndicator.vertical: ScrollIndicator {}

        Text {
            id: titleText
            anchors {
                left: parent.left
                right: parent.right
                margins: 0
            }

            text: "
# ArcGIS Maps SDK for Qt Sample Viewer

The ArcGIS Maps SDK for Qt Sample Viewer – C++ presents an overview of what you can achieve using the ArcGIS Maps SDK for Qt in your Qt applications.
This application contains several samples that demonstrate the power and ease-of-use of the ArcGIS Maps SDK for Qt product, and more samples are coming.
You can browse all the samples by navigating through the different categories.
"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText

        }

        Text {
            id: featuredSamplesText
            anchors {
                top: titleText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                margins: 0
            }
            text: "### Featured Samples"
            wrapMode: Text.WordWrap
            textFormat: Text.MarkdownText
        }

        GridView {
            id: featuredSamplesGrid
            anchors {
                top: featuredSamplesText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            cellWidth: 150
            cellHeight: 150
            clip: true
            model: SampleManager.currentCategory.samples
            delegate: Item {
                height: 150
                width: 150
                clip: true

                Image {
                    height: featuredSamplesGrid.cellWidth
                    width: featuredSamplesGrid.cellWidth
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: thumbnailUrl
                }

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: name
                    wrapMode: Text.WordWrap
                    styleColor: "#ffffff"
                    style: Text.Outline
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
