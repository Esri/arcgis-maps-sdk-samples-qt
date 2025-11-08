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
        if (!manageOfflineDataViewPage.visible && SampleManager.downloadInProgress)
            SampleManager.cancelDownload = true;
        else if (manageOfflineDataViewPage.visible)
            SampleManager.getOfflineDataProjects();
    }

    Connections {
        target: SampleManager
        function onDoneDownloadingChanged() {
            SampleManager.getOfflineDataProjects();
        }
    }

    property bool showOnlyDownloaded: false
    property real scaleFactor: Math.min(1.0, Math.max(0.5, Math.min(width / 800, height / 600)))
    property real baseFontSize: 12 * scaleFactor
    property real baseSpacing: 10 * scaleFactor
    property real baseMargin: Math.max(15, 20 * scaleFactor)

    // Full-page download view (only for bulk "Download all" operations)
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: baseMargin
        spacing: baseSpacing
        visible: SampleManager.downloadInProgress && SampleManager.isBulkDownload

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            Layout.preferredWidth: Math.max(100, 140 * scaleFactor)
            Layout.preferredHeight: Math.max(100, 140 * scaleFactor)
            fillMode: Image.PreserveAspectFit
            clip: true
        }

        Label {
            text: SampleManager.downloadText
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
            text: "%1% complete".arg(SampleManager.downloadProgress)
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            visible: SampleManager.downloadInProgress
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
            value: SampleManager.downloadProgress
            visible: SampleManager.downloadInProgress
            clip: true
        }

        Button {
            text: SampleManager.cancelDownload ? qsTr("Cancelling remaining downloads...") : qsTr("Cancel remaining downloads")
            enabled: !SampleManager.cancelDownload
            font.pixelSize: Math.max(10, baseFontSize)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
            Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
            onClicked: {
                SampleManager.cancelDownload = true;
            }
            clip: true
        }
    }

    // Main list view
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: baseMargin
        spacing: baseSpacing
        visible: !(SampleManager.downloadInProgress && SampleManager.isBulkDownload)

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            Layout.preferredWidth: Math.max(60, 100 * scaleFactor)
            Layout.preferredHeight: Math.max(60, 100 * scaleFactor)
            fillMode: Image.PreserveAspectFit
            clip: true
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: baseSpacing

            Button {
                text: SampleManager.downloadFailed ? qsTr("Retry download all offline data") : qsTr("Download all offline data")
                Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
                Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
                enabled: SampleManager.hasAnyDataToDownload
                onClicked: {
                    if (SampleManager.reachability === SampleManager.ReachabilityOnline || SampleManager.reachability === SampleManager.ReachabilityUnknown) {
                        SampleManager.downloadAllDataItems();
                    } else {
                        SampleManager.currentMode = SampleManager.NetworkRequiredView;
                    }
                }
                font.pixelSize: Math.max(10, baseFontSize)
                clip: true
            }

            Button {
                text: qsTr("Delete all offline data")
                Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
                Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
                enabled: SampleManager.hasAnyDataToDelete
                onClicked: {
                    deleteAllDialog.open();
                }
                font.pixelSize: Math.max(10, baseFontSize)
                clip: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: baseSpacing * 1.25
            Layout.bottomMargin: baseSpacing * 1.25
            Layout.leftMargin: baseMargin
            Layout.rightMargin: baseMargin
            spacing: baseSpacing

            Label {
                Layout.fillWidth: true
                text: SampleManager.downloadStatusText
                visible: SampleManager.downloadInProgress && SampleManager.downloadStatusText !== ""
                color: Calcite.text1
                font.pixelSize: Math.max(10, baseFontSize)
                elide: Text.ElideRight
            }

            Item {
                Layout.fillWidth: true
                visible: !SampleManager.downloadInProgress
            }

            CheckBox {
                text: qsTr("Show only downloaded data")
                checked: showOnlyDownloaded
                enabled: SampleManager.hasAnyDataToDelete
                onClicked: {
                    showOnlyDownloaded = checked;
                }
                onEnabledChanged: {
                    if (!enabled) {
                        showOnlyDownloaded = false;
                    }
                }
                font.pixelSize: Math.max(10, baseFontSize)
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            border {
                color: Calcite.border1
                width: 2
            }
            radius: 8

            ScrollView {
                anchors.fill: parent
                anchors.margins: 5
                clip: true

                ListView {
                    model: SampleManager.offlineDataProjects
                    spacing: 5

                    Component.onCompleted: {
                        SampleManager.getOfflineDataProjects();
                    }

                    delegate: Rectangle {
                        visible: !showOnlyDownloaded || model.downloaded
                        width: ListView.view.width
                        height: Math.max(65, 80 * scaleFactor)
                        color: Calcite.foreground2
                        radius: 8
                        border {
                            color: Calcite.border2
                            width: 1
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors {
                                margins: baseMargin
                                topMargin: 2
                                bottomMargin: 2
                            }
                            spacing: baseSpacing / 2

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                spacing: baseSpacing

                                Column {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    Label {
                                        text: model.sample.name
                                        font {
                                            family: fontFamily
                                            weight: Font.DemiBold
                                            pixelSize: Math.max(11, baseFontSize + 1)
                                        }
                                        elide: Label.ElideRight
                                        width: parent.width
                                        wrapMode: Text.WordWrap
                                        maximumLineCount: 2
                                    }

                                    Rectangle {
                                        width: statusLabel.implicitWidth + 20
                                        height: statusLabel.implicitHeight + 10
                                        color: {
                                            if (model.downloading) {
                                                return Calcite.warning;
                                            } else if (model.downloaded) {
                                                return Calcite.success;
                                            } else if (model.downloadedItemsCount > 0 && model.downloadedItemsCount < model.totalItemsCount) {
                                                return "#FFA500";  // Orange for partial downloads
                                            } else {
                                                return Calcite.danger;
                                            }
                                        }
                                        radius: 12

                                        Label {
                                            id: statusLabel
                                            anchors.centerIn: parent
                                            text: {
                                                if (model.downloading) {
                                                    return qsTr("Downloading...");
                                                } else if (model.downloaded) {
                                                    return qsTr("Downloaded");
                                                } else if (model.downloadedItemsCount > 0 && model.downloadedItemsCount < model.totalItemsCount) {
                                                    return model.downloadedItemsCount + "/" + model.totalItemsCount + " " + qsTr("Downloaded");
                                                } else {
                                                    return qsTr("Not downloaded");
                                                }
                                            }
                                            color: Calcite.text1
                                            font {
                                                family: fontFamily
                                                pixelSize: Math.max(9, baseFontSize - 2)
                                                weight: Font.Medium
                                            }
                                        }
                                    }

                                    ProgressBar {
                                        width: parent.width
                                        visible: model.downloading
                                        from: 0
                                        to: 100
                                        value: model.downloadProgress
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                RowLayout {
                                    spacing: baseSpacing / 2

                                    Button {
                                        text: qsTr("Download")
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: model.downloadedItemsCount < model.totalItemsCount && !model.downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            SampleManager.downloadProjectData(model.sample.name);
                                        }
                                    }

                                    Button {
                                        text: qsTr("Cancel")
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: model.downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            SampleManager.cancelSampleDownload(model.sample.name);
                                        }
                                    }

                                    Button {
                                        text: qsTr("Delete")
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: model.downloadedItemsCount > 0 && !model.downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            deleteProjectDialog.sampleName = model.sample.name;
                                            deleteProjectDialog.resetDialog();
                                            deleteProjectDialog.visible = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

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
}
