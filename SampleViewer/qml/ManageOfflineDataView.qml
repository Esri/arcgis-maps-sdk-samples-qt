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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: baseMargin
        spacing: baseSpacing

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            Layout.preferredWidth: Math.max(60, 100 * scaleFactor)
            Layout.preferredHeight: Math.max(60, 100 * scaleFactor)
            fillMode: Image.PreserveAspectFit
            clip: true
            visible: !SampleManager.showFullScreenDownload
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: baseMargin
            spacing: baseSpacing
            visible: SampleManager.downloadInProgress && SampleManager.showFullScreenDownload

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

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            visible: !SampleManager.showFullScreenDownload
            spacing: baseSpacing

            Button {
                text: SampleManager.downloadFailed ? qsTr("Retry download all offline data") : qsTr("Download all offline data")
                Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
                Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
                enabled: {
                    if (!SampleManager.offlineDataProjects)
                        return false;

                    for (let i = 0; i < SampleManager.offlineDataProjects.rowCount(); i++) {
                        let downloaded = SampleManager.offlineDataProjects.data(
                                SampleManager.offlineDataProjects.index(i),
                                SampleManager.offlineDataProjects.DownloadedRole
                                );
                        if (!downloaded)
                            return true;
                    }
                    return false;
                }
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
                enabled: {
                    if (!SampleManager.offlineDataProjects)
                        return false;

                    for (let i = 0; i < SampleManager.offlineDataProjects.rowCount(); i++) {
                        let downloaded = SampleManager.offlineDataProjects.data(
                                SampleManager.offlineDataProjects.index(i),
                                SampleManager.offlineDataProjects.DownloadedRole
                                );
                        if (downloaded)
                            return true;
                    }
                    return false;
                }
                onClicked: {
                    deleteAllDialog.visible = true;
                }
                font.pixelSize: Math.max(10, baseFontSize)
                clip: true
            }
        }

        CheckBox {
            text: qsTr("Show only downloaded data")
            Layout.alignment: Qt.AlignRight
            visible: !SampleManager.showFullScreenDownload
            checked: showOnlyDownloaded
            enabled: {
                if (!SampleManager.offlineDataProjects)
                    return false;

                for (let i = 0; i < SampleManager.offlineDataProjects.rowCount(); i++) {
                    let downloaded = SampleManager.offlineDataProjects.data(
                            SampleManager.offlineDataProjects.index(i),
                            SampleManager.offlineDataProjects.DownloadedRole
                            );
                    if (downloaded)
                        return true;
                }
                return false;
            }
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

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !SampleManager.showFullScreenDownload
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
                        width: ListView.view.width
                        height: visible ? (downloading ? Math.max(100, 120 * scaleFactor) : Math.max(65, 80 * scaleFactor)) : 0
                        visible: !showOnlyDownloaded || downloaded
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
                                        text: sample.name
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
                                        color: downloaded ? Calcite.success : (downloading ? Calcite.warning : Calcite.danger)
                                        radius: 12

                                        Label {
                                            id: statusLabel
                                            anchors.centerIn: parent
                                            text: {
                                                if (downloaded)
                                                    return qsTr("Downloaded")
                                                else if (downloading)
                                                    return qsTr("Downloading...")
                                                else
                                                    return qsTr("Not downloaded")
                                            }
                                            color: Calcite.text1
                                            font {
                                                family: fontFamily
                                                pixelSize: Math.max(9, baseFontSize - 2)
                                                weight: Font.Medium
                                            }
                                        }
                                    }

                                    ColumnLayout {
                                        width: parent.width
                                        spacing: 2
                                        visible: downloading

                                        Label {
                                            text: qsTr("%1 of %2 items").arg(downloadedItemsCount).arg(totalItemsCount)
                                            font {
                                                family: fontFamily
                                                pixelSize: Math.max(9, baseFontSize - 2)
                                            }
                                            color: Calcite.text2
                                        }

                                        ProgressBar {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 6
                                            from: 0
                                            to: 1.0
                                            value: downloadProgress
                                        }

                                        Label {
                                            text: qsTr("%1% complete").arg(Math.round(downloadProgress * 100))
                                            font {
                                                family: fontFamily
                                                pixelSize: Math.max(9, baseFontSize - 2)
                                            }
                                            color: Calcite.text2
                                        }
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
                                        visible: !downloaded && !downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            SampleManager.downloadProjectData(sample.name);
                                        }
                                    }

                                    Button {
                                        text: qsTr("Cancel")
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            SampleManager.cancelSampleDownload(sample.name);
                                        }
                                    }

                                    Button {
                                        text: qsTr("Delete")
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: downloaded && !downloading
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            deleteProjectDialog.sampleName = sample.name;
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
