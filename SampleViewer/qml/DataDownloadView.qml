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
import QtQuick.Layouts

Page {
    id: page
    visible: SampleManager.currentMode === SampleManager.DownloadDataView
    property bool pageDownloadInProgress: false

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            clip: true
        }

        Label {
            id: networkConnectionText
            text: SampleManager.downloadsManager.downloadInProgress ? qsTr("Currently downloading offline data.") : qsTr("Offline Data Required")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {
                family: fontFamily
                weight: Font.DemiBold
            }
            Layout.fillWidth: true
            clip: true
        }

        Label {
            text:  SampleManager.downloadsManager.downloadFailed ? SampleManager.downloadsManager.downloadText : qsTr("This sample uses offline data that is not detected on your system. Press the 'Download' button to download the data to your device.")
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            visible: !SampleManager.downloadsManager.downloadInProgress
            font {
                family: fontFamily
            }
            clip: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            text: (SampleManager.downloadsManager.downloadFailed) ? qsTr("Retry") : qsTr("Download")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: !SampleManager.downloadsManager.downloadInProgress
            onClicked: {
                if (SampleManager.reachability === SampleManager.ReachabilityOnline || SampleManager.reachability === SampleManager.ReachabilityUnknown) {
                    SampleManager.downloadDataItemsCurrentSample();
                } else {
                    SampleManager.currentMode = SampleManager.NetworkRequiredView;
                }
            }
            clip: true
        }

        Label {
            text: SampleManager.downloadsManager.downloadText
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Label.AlignHCenter
            visible: SampleManager.downloadsManager.downloadInProgress
            font {
                family: fontFamily
            }
            clip: true
        }

        Label {
            text: qsTr("%1% complete").arg(SampleManager.downloadsManager.downloadProgress)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            visible: SampleManager.downloadsManager.downloadInProgress
            font {
                family: fontFamily
            }
            clip: true
        }

        ProgressBar {
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            from: 0
            to: 100
            value: SampleManager.downloadsManager.downloadProgress
            visible: SampleManager.downloadsManager.downloadInProgress
            clip: true
        }

        Button {
            text: qsTr("Cancel")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: SampleManager.downloadsManager.downloadInProgress
            onClicked: SampleManager.cancelAllDownloads()
            clip: true
        }
    }
}
