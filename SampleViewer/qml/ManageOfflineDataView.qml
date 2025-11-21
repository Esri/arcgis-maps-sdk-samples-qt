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
import Qt.labs.platform
import QtQuick.Layouts
import Esri.ArcGISRuntimeSamples
import Calcite

Page {
    id: manageOfflineDataViewPage
    visible: SampleManager.currentMode === SampleManager.ManageOfflineDataView

    background: Rectangle {
        color: Calcite.foreground1
    }

    onVisibleChanged: {
        if (!manageOfflineDataViewPage.visible && SampleManager.downloadsManager.downloadInProgress)
            SampleManager.downloadsManager.cancelDownload = true;
        else if (manageOfflineDataViewPage.visible)
            SampleManager.getOfflineDataProjects();
    }

    Connections {
        target: SampleManager.downloadsManager
        function onDoneDownloadingChanged() {
            SampleManager.getOfflineDataProjects();
        }
    }

    property bool showOnlyDownloaded: false
    property real scaleFactor: Math.min(1.0, Math.max(0.5, Math.min(width / 800, height / 600)))
    property real baseFontSize: 12 * scaleFactor
    property real baseSpacing: 10 * scaleFactor
    property real baseMargin: Math.max(15, 20 * scaleFactor)

    // Full page download view - bulk download
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: baseMargin
        spacing: baseSpacing
        visible: SampleManager.downloadsManager.downloadInProgress && SampleManager.downloadsManager.isBulkDownload

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            Layout.preferredWidth: Math.max(100, 140 * scaleFactor)
            Layout.preferredHeight: Math.max(100, 140 * scaleFactor)
            fillMode: Image.PreserveAspectFit
            clip: true
        }

        Label {
            text: SampleManager.downloadsManager.downloadText
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            font {
                family: fontFamily
                pixelSize: Math.max(11, baseFontSize)
            }
            wrapMode: Text.WordWrap
            clip: true
        }

        Label {
            text: "%1% complete".arg(SampleManager.downloadsManager.downloadProgress)
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            visible: SampleManager.downloadsManager.downloadInProgress
            font {
                family: fontFamily
                pixelSize: Math.max(13, baseFontSize + 2)
                weight: Font.DemiBold
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
            text: qsTr("Cancel all downloads")
            font.pixelSize: Math.max(10, baseFontSize)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
            Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
            onClicked: {
                SampleManager.cancelAllDownloads();
            }
            clip: true
        }
    }

    // Individual sample list
    OfflineDataListView {
        visible: !(SampleManager.downloadsManager.downloadInProgress && SampleManager.downloadsManager.isBulkDownload)
        scaleFactor: manageOfflineDataViewPage.scaleFactor
        baseMargin: manageOfflineDataViewPage.baseMargin
        baseSpacing: manageOfflineDataViewPage.baseSpacing
        baseFontSize: manageOfflineDataViewPage.baseFontSize
        deleteProjectDialog: deleteProjectDialog
        deleteAllDialog: deleteAllDialog
        showOnlyDownloaded: manageOfflineDataViewPage.showOnlyDownloaded
        onShowOnlyDownloadedChanged: manageOfflineDataViewPage.showOnlyDownloaded = showOnlyDownloaded
    }

    // Delete all offline data confirmation dialog
    MessageDialog {
        id: deleteAllDialog
        title: qsTr("Delete all offline data")
        text: qsTr("Are you sure you want to delete all offline data?")
        visible: false
        buttons: MessageDialog.Yes | MessageDialog.No

        function resetDialog() {
            text = qsTr("Are you sure you want to delete all offline data?");
            buttons = MessageDialog.Yes | MessageDialog.No;
            visible = false;
        }

        onClicked: function(button) {
            if (button === MessageDialog.Yes){
                if (SampleManager.deleteAllOfflineData()) {
                    text = qsTr("Project data deleted successfully");
                    buttons = MessageDialog.Ok;
                    visible = true;
                } else {
                    text = qsTr("Failed to delete project data");
                    buttons = MessageDialog.Ok;
                    visible = true;
                }
            }
            else if (button === MessageDialog.No){
                visible = false;
            }
            else if (button === MessageDialog.Ok){
                resetDialog();
            }
        }
    }

    // Delete individual project data confirmation dialog
    MessageDialog {
        id: deleteProjectDialog
        property string sampleName: ""
        title: qsTr("Delete project data")
        text: qsTr("Are you sure you want to delete offline data for '") + sampleName + "'?"
        visible: false
        buttons: MessageDialog.Yes | MessageDialog.No

        function resetDialog() {
            text = qsTr("Are you sure you want to delete offline data for '") + sampleName + "'?";
            buttons = MessageDialog.Yes | MessageDialog.No;
            visible = false;
        }

        onClicked: function(button) {
            if (button === MessageDialog.Yes){
                if (SampleManager.deleteProjectOfflineData(sampleName)) {
                    text = qsTr("Project data deleted successfully");
                    buttons = MessageDialog.Ok;
                    visible = true;
                }
                else {
                    text = qsTr("Failed to delete project data");
                    buttons = MessageDialog.Ok;
                    visible = true;
                }
            }
            else if (button === MessageDialog.No){
                visible = false;
            }
            else if (button === MessageDialog.Ok){
                resetDialog();
            }
        }
    }

    // Busy overlay for cancelling downloads
    Rectangle {
        anchors.fill: parent
        color: "#CC000000"  // Dark semi-transparent background
        visible: SampleManager.downloadsManager.cancelDownload && SampleManager.downloadsManager.downloadInProgress
        z: 1000  // Ensure it's on top of everything

        ColumnLayout {
            anchors.centerIn: parent
            spacing: baseSpacing * 2

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: parent.parent.visible
                Layout.preferredWidth: Math.max(60, 80 * scaleFactor)
                Layout.preferredHeight: Math.max(60, 80 * scaleFactor)
            }

            Label {
                text: qsTr("Canceling remaining downloads...")
                color: Calcite.offWhite
                font {
                    family: fontFamily
                    pixelSize: Math.max(12, baseFontSize + 2)
                    weight: Font.Medium
                }
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
