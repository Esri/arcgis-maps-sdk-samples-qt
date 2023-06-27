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
import Telemetry

ListView {
    spacing: 10
    delegate: ItemDelegate {
        id: searchDelegate
        width: parent ? parent.width : 0
        height: 45
        clip: true
        text: name

        contentItem: Label {
            text: searchDelegate.text
            font: searchDelegate.font
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
                color: "gray"
                width: 1
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                drawer.close();
                // launch sample...
                SampleManager.currentSample = sample;
                Telemetry.postEvent("sample_opened", {"sample_name": sample.name, "referrer": "search"});
            }
        }
    }
}
