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
            SampleManager.cancelAllDownloads();
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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: SampleManager.downloadsManager.downloadInProgress && SampleManager.downloadsManager.isBulkDownload

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            clip: true
        }

        Label {
            text: qsTr("Currently downloading all offline data.")
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
            text: SampleManager.downloadsManager.downloadText
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Label.AlignHCenter
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
            visible: !SampleManager.downloadsManager.cancelDownload
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
            visible: !SampleManager.downloadsManager.cancelDownload
            clip: true
        }

        Button {
            // PortalItem::fetchData does not have a cancel method so we can only clear the remaining items from the download queue
            id: cancelAllButton
            text: qsTr("Cancel remaining downloads")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            padding: 12
            onClicked: {
                SampleManager.cancelAllDownloads();
            }
            clip: true

            background: Rectangle {
                radius: 4
                color: cancelAllButton.down ? Qt.darker(Calcite.brand, 1.2) : (cancelAllButton.hovered ? Calcite.brandHover : Calcite.brand)
            }
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
        onShowOnlyDownloadedChanged: {
            manageOfflineDataViewPage.showOnlyDownloaded = showOnlyDownloaded;
        }
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
        color: "#CC000000"
        visible: SampleManager.downloadsManager.cancelDownload && SampleManager.downloadsManager.downloadInProgress
        z: 1000

        ColumnLayout {
            anchors.centerIn: parent
            spacing: baseSpacing * 2

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: parent.parent.visible
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
