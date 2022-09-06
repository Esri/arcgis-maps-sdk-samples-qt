// [Legal]
// Copyright 2022 Esri.

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
import Qt5Compat.GraphicalEffects
import Esri.ArcGISRuntimeSamples
import Telemetry

Component {
    id: categoryDelegate

    ItemDelegate {
        width: 105
        height: 105
        clip: true

        background: Image {
            id: categoryImg
            anchors.fill: parent
            source: backgroundThumbnailUrl
            clip: true

            Rectangle {
                anchors.fill: parent
                clip: true
                opacity: 0.4
                color: "black"
                radius: 10
            }
        }

        contentItem: Item {
            anchors.fill: parent

            Column {
                anchors.centerIn: parent
                width: parent.width
                spacing: 2

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 30
                    height: width
                    radius: 30
                    color: "#333333"

                    Image {
                        id: thumbnailImage
                        anchors.centerIn: parent
                        width: 18
                        height: width
                        source: thumbnailUrl
                        clip: true
                        visible: drawer.visible
                    }

                    ColorOverlay {
                        anchors.fill: thumbnailImage
                        source: thumbnailImage
                        color: "white"
                    }
                }

                Text {
                    width: parent.width
                    antialiasing: true
                    text: displayName
                    visible: drawer.visible
                    color: "white"
                    font {
                        capitalization: Font.AllUppercase
                        pixelSize: 11
                        weight: Font.DemiBold
                        family: fontFamily
                    }
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        onClicked: {
            sampleListView.currentCategory = displayName;
            SampleManager.currentCategory = SampleManager.categories.get(index);
            stackView.push(sampleListView);
            GAnalytics.postEvent("category_selected", {"category_name": displayName});
        }
    }
}
