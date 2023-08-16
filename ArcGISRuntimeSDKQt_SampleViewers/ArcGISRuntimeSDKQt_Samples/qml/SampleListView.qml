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
import Esri.ArcGISRuntimeSamples
<<<<<<< HEAD
=======
import Telemetry
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd

Page {
    id: sampleListView
    visible: stackView.currentItem === this

    property string currentCategory: ""

    header: ToolBar {
        height: 42

        Image {
            id: back
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            source: "qrc:/back.png"
            width: 32
            height: width

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    stackView.pop();
                }
            }
        }

        Label {
            anchors.centerIn: parent
            text: currentCategory
            font {
                pixelSize: 18
                family: fontFamily
            }
            color: "white"
        }
    }

    Rectangle {
        id: sampleListRect
        anchors {
            fill: parent
            margins: 10
        }
        color: "transparent"

        ListView {
            anchors.fill: parent
            clip: true
<<<<<<< HEAD
            model: SampleManager.currentCategory ? SampleManager.currentCategory.samples : []
=======
            model: SampleManager.currentCategory.samples
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
            spacing: 10
            delegate: ItemDelegate {
                id: itemDelegate
                width: parent ? parent.width : 0
                height: 45
                clip: true
                text: name

                contentItem: Label {
                    text: itemDelegate.text
                    font: itemDelegate.font
                    elide: Text.ElideNone
                    wrapMode: Text.Wrap
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    width: parent.width
                    height: parent.height
                    color: "transparent"
                    radius: 4
                    opacity: 0.9
                    border {
                        color: "darkgray"
                        width: 1
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        drawer.close();
                        // launch sample...
                        SampleManager.currentSample = sample;
<<<<<<< HEAD
=======
                        GAnalytics.postEvent("sample_opened", {"sample_name": sample.name, "referrer": "category list"});
>>>>>>> 859758696bdde217ee54ce2bb721268439b882dd
                    }
                }
            }
        }
    }
}
