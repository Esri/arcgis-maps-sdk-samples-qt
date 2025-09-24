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

        Label {
            text: qsTr("Currently downloading offline data.")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            horizontalAlignment: Label.AlignHCenter
            font {
                family: fontFamily
                weight: Font.DemiBold
                pixelSize: Math.max(12, baseFontSize + 2)
            }
            Layout.fillWidth: true
            clip: true
            visible: SampleManager.downloadInProgress
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
                    if (SampleManager.offlineDataProjects.length === 0) return false;
                    for (let i = 0; i < SampleManager.offlineDataProjects.length; i++) {
                        if (!SampleManager.offlineDataProjects[i].downloaded) return true;
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
                    if (SampleManager.offlineDataProjects.length === 0) return false;
                    for (let i = 0; i < SampleManager.offlineDataProjects.length; i++) {
                        if (SampleManager.offlineDataProjects[i].downloaded) return true;
                    }
                    return false;
                }
                onClicked: {
                    deleteAllDialog.visible = SampleManager.deleteAllOfflineData();
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
                if (SampleManager.offlineDataProjects.length === 0) return false;
                for (let i = 0; i < SampleManager.offlineDataProjects.length; i++) {
                    if (SampleManager.offlineDataProjects[i].downloaded) return true;
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
                        height: {
                            var parentWidth = parent ? parent.width : ListView.view.width;
                            var parentHeight = parent ? parent.height : ListView.view.height;
                            return (parentWidth > 400 && parentWidth > parentHeight) ?
                                        Math.max(45, 60 * scaleFactor) :
                                        Math.max(80, 100 * scaleFactor);
                        }
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

                                Loader {
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                    sourceComponent: (parent.width > 400 && parent.width > parent.height) ? rowButtonsComponent : columnButtonsComponent

                                    Component {
                                        id: rowButtonsComponent
                                        RowLayout {
                                            spacing: baseSpacing / 2

                                            Button {
                                                text: "Download"
                                                Layout.preferredWidth: Math.max(100, 120 * scaleFactor)
                                                Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                                enabled: !modelData.downloaded && !SampleManager.downloadInProgress
                                                font.pixelSize: Math.max(9, baseFontSize - 1)
                                                onClicked: {
                                                    SampleManager.downloadProjectData(modelData.sample.name);
                                                }
                                            }

                                            Button {
                                                text: "Delete"
                                                Layout.preferredWidth: Math.max(100, 120 * scaleFactor)
                                                Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                                enabled: modelData.downloaded
                                                font.pixelSize: Math.max(9, baseFontSize - 1)
                                                onClicked: {
                                                    deleteProjectDialog.sampleName = modelData.sample.name;
                                                    deleteProjectDialog.visible = true;
                                                }
                                            }
                                        }
                                    }

                                    Component {
                                        id: columnButtonsComponent
                                        ColumnLayout {
                                            anchors.centerIn: parent
                                            spacing: 0

                                            Button {
                                                text: "Download"
                                                Layout.preferredWidth: Math.max(95, 115 * scaleFactor)
                                                Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                                enabled: !modelData.downloaded && !SampleManager.downloadInProgress
                                                font.pixelSize: Math.max(9, baseFontSize - 1)
                                                onClicked: {
                                                    SampleManager.downloadProjectData(modelData.sample.name);
                                                }
                                            }

                                            Button {
                                                text: "Delete"
                                                Layout.preferredWidth: Math.max(95, 115 * scaleFactor)
                                                Layout.preferredHeight: Math.max(32, 40 * scaleFactor)
                                                enabled: modelData.downloaded
                                                font.pixelSize: Math.max(9, baseFontSize - 1)
                                                onClicked: {
                                                    deleteProjectDialog.sampleName = modelData.sample.name;
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
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(120, 150 * scaleFactor)
            visible: SampleManager.downloadInProgress || SampleManager.downloadFailed
            color: "#ffffff"
            border.color: "#dee2e6"
            border.width: 1
            radius: 8

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: baseMargin
                spacing: baseSpacing

                Label {
                    text: SampleManager.downloadText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: true
                    horizontalAlignment: Label.AlignHCenter
                    font {
                        family: fontFamily
                        pixelSize: Math.max(11, baseFontSize)
                    }
                    wrapMode: Text.WordWrap
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
                        pixelSize: Math.max(13, baseFontSize + 2)
                        weight: Font.DemiBold
                    }
                    clip: true
                }

                ProgressBar {
                    Layout.fillHeight: false
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.max(8, 12 * scaleFactor)
                    from: 0
                    to: 100
                    value: SampleManager.downloadProgress
                    visible: SampleManager.downloadInProgress
                    clip: true
                }

                Button {
                    text: SampleManager.cancelDownload ? "Cancelling remaining downloads..." : "Cancel remaining downloads"
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.preferredWidth: Math.max(200, 250 * scaleFactor)
                    Layout.preferredHeight: Math.max(36, 45 * scaleFactor)
                    visible: SampleManager.downloadInProgress
                    enabled: !SampleManager.cancelDownload
                    font.pixelSize: Math.max(10, baseFontSize)
                    onClicked: {
                        SampleManager.cancelDownload = true;
                    }
                    clip: true
                }
            }
        }
    }

    MessageDialog {
        id: deleteAllDialog
        title: "Delete all offline data"
        text: "Delete all offline data was successful"
        visible: false
        onRejected: {
            visible = false;
        }
    }

    MessageDialog {
        id: deleteProjectDialog
        property string sampleName: ""
        title: "Delete project data"
        text: "Are you sure you want to delete offline data for '" + sampleName + "'?"
        visible: false
        buttons: MessageDialog.Yes | MessageDialog.No
        onAccepted: {
            if (SampleManager.deleteProjectOfflineData(sampleName)) {
                text = "Project data deleted successfully";
                buttons = MessageDialog.Ok;
            } else {
                text = "Failed to delete project data";
                buttons = MessageDialog.Ok;
            }
        }
        onRejected: {
            visible = false;
        }
    }
}
