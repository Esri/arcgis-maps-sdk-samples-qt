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
import Esri.ArcGISExtras
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
            text: SampleManager.downloadInProgress ? qsTr("Currently downloading offline data.") : qsTr("Offline Data Required")
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
            text: qsTr("This sample uses offline data that is not detected on your system. Press the 'Download' button to download the data to your device.")
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            visible: !SampleManager.downloadInProgress
            font {
                family: fontFamily
            }
            clip: true
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            text: (dataDownloadLoader.item && dataDownloadLoader.item.failedToDownload) ? qsTr("Retry") : qsTr("Download")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: !SampleManager.downloadInProgress
            onClicked: {
                if (System.reachability === System.ReachabilityOnline || System.reachability === System.ReachabilityUnknown) {
                    dataDownloadLoader.item.downloadDataItems();
                    pageDownloadInProgress = true;
                } else {
                    SampleManager.currentMode = SampleManager.NetworkRequiredView;
                }
            }
            clip: true
        }

        Label {
            text: SampleManager.downloadText
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Label.AlignHCenter
            visible: SampleManager.downloadInProgress || (dataDownloadLoader.item && dataDownloadLoader.item.failedToDownload)
            font {
                family: fontFamily
            }
            clip: true
        }

        Label {
            text: "%1% complete".arg(SampleManager.downloadProgress)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            visible: SampleManager.downloadInProgress
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
            value: SampleManager.downloadProgress
            visible: SampleManager.downloadInProgress
            clip: true
        }
    }

    Loader {
        id: dataDownloadLoader
        sourceComponent: page.visible || pageDownloadInProgress ? dataDownloaderComponent : undefined
    }

    Component {
        id: dataDownloaderComponent
        DataDownloader {
        }
    }

    Connections {
        target: SampleManager

        function onDoneDownloadingChanged () {
            pageDownloadInProgress = false;
        }
    }
}
