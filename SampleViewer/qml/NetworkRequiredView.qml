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

Page {
    visible: SampleManager.currentMode === SampleManager.NetworkRequiredView

    Column {
        anchors.centerIn: parent
        spacing: 25

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/logo.png"
            width: 72
            height: width
        }

        Label {
            id: networkConnectionText
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Network Connection Required")
            renderType: Text.NativeRendering
            font {
                family: fontFamily
                weight: Font.DemiBold
                pixelSize: 18
            }
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            width: networkConnectionText.width + 20
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            text: qsTr("This sample requires network connectivity but your system is not online. Please acquire a network connection or select an offline sample instead.")
            renderType: Text.NativeRendering
            horizontalAlignment: Text.AlignHCenter
            font {
                family: fontFamily
                pixelSize: 14
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 150
            height: 48
            text: qsTr("Retry")
            onClicked: {
                if (SampleManager.reachability === SampleManager.ReachabilityOnline) {
                    SampleManager.currentMode = SampleManager.LiveSampleView
                    showSample();
                }
            }
        }
    }
}
