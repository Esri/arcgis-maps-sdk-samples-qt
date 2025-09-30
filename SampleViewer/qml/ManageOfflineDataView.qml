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

Page {
    id: manageOfflineDataViewPage
    visible: SampleManager.currentMode === SampleManager.ManageOfflineDataView

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
    property real scaleFactor: Math.min(width / 800, height / 600)
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
            visible: !SampleManager.downloadInProgress
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: baseMargin
            spacing: baseSpacing
            visible: SampleManager.downloadInProgress

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
                text: SampleManager.cancelDownload ? "Cancelling remaining downloads..." : "Cancel remaining downloads"
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
            visible: !SampleManager.downloadInProgress
            spacing: baseSpacing

            Button {
                text: SampleManager.downloadFailed ? "Retry download all offline data" : "Download all offline data"
                Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
                Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
                enabled: {
                    return SampleManager.offlineDataProjects &&
                            SampleManager.offlineDataProjects.some(project => !project.downloaded);
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
                    return SampleManager.offlineDataProjects.length > 0 &&
                            SampleManager.offlineDataProjects.some(project => project.downloaded);
                }
                onClicked: {
                    deleteAllDialog.visible = true;
                }
                font.pixelSize: Math.max(10, baseFontSize)
                clip: true
            }
        }

        CheckBox {
            text: "Show only downloaded data"
            Layout.alignment: Qt.AlignRight
            visible: !SampleManager.downloadInProgress
            checked: showOnlyDownloaded
            enabled: {
                return SampleManager.offlineDataProjects.length > 0 &&
                        SampleManager.offlineDataProjects.some(project => project.downloaded);
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
            visible: !SampleManager.downloadInProgress
            color: "transparent"
            border {
                color: "darkgray"
                width: 2
            }
            radius: 8

            ScrollView {
                anchors.fill: parent
                anchors.margins: 5
                clip: true

                ListView {
                    model: {
                        if (!showOnlyDownloaded) {
                            return SampleManager.offlineDataProjects;
                        } else {
                            return SampleManager.offlineDataProjects.filter(function(item) {
                                return item.downloaded;
                            });
                        }
                    }
                    spacing: 5

                    Component.onCompleted: {
                        SampleManager.getOfflineDataProjects();
                    }

                    delegate: Rectangle {
                        width: ListView.view.width
                        height: Math.max(65, 80 * scaleFactor)
                        color: "#d1d5db"
                        radius: 8
                        border {
                            color: "#6b7280"
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
                                        text: modelData.sample.name
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
                                        color: modelData.downloaded ? "#d4edda" : "#f8d7da"
                                        radius: 12

                                        Label {
                                            id: statusLabel
                                            anchors.centerIn: parent
                                            text: modelData.downloaded ? "Downloaded" : "Not downloaded"
                                            color: modelData.downloaded ? "#155724" : "#721c24"
                                            font {
                                                family: fontFamily
                                                pixelSize: Math.max(9, baseFontSize - 2)
                                                weight: Font.Medium
                                            }
                                        }
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                RowLayout {
                                    spacing: baseSpacing / 2

                                    Button {
                                        text: "Download"
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: !modelData.downloaded
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            SampleManager.downloadProjectData(modelData.sample.name);
                                        }
                                    }

                                    Button {
                                        text: "Delete"
                                        Layout.preferredWidth: Math.max(110, 110 * scaleFactor)
                                        Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                        visible: modelData.downloaded
                                        font.pixelSize: Math.max(12, baseFontSize - 1)
                                        onClicked: {
                                            console.log(modelData.sample.name)
                                            deleteProjectDialog.sampleName = modelData.sample.name;
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
        title: "Delete all offline data"
        text: "Are you sure you want to delete all offline data?"
        visible: false
        buttons: MessageDialog.Yes | MessageDialog.No

        function resetDialog() {
            text = "Are you sure you want to delete all offline data?";
            buttons = MessageDialog.Yes | MessageDialog.No;
            visible = false;
        }

        onClicked: function(button) {
            if (button === MessageDialog.Yes){
                if (SampleManager.deleteAllOfflineData()) {
                    text = "Project data deleted successfully";
                    buttons = MessageDialog.Ok;
                    visible = true;
                } else {
                    text = "Failed to delete project data";
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
        title: "Delete project data"
        text: "Are you sure you want to delete offline data for '" + sampleName + "'?"
        visible: false
        buttons: MessageDialog.Yes | MessageDialog.No

        function resetDialog() {
            text = "Are you sure you want to delete offline data for '" + sampleName + "'?";
            buttons = MessageDialog.Yes | MessageDialog.No;
            visible = false;
        }

        onClicked: function(button) {
            if (button === MessageDialog.Yes){
                if (SampleManager.deleteProjectOfflineData(sampleName)) {
                    text = "Project data deleted successfully";
                    buttons = MessageDialog.Ok;
                    visible = true;
                }
                else {
                    text = "Failed to delete project data";
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
