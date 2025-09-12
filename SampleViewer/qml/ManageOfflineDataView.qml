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

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        Image {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            source: "qrc:/logo.png"
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100
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
            }
            Layout.fillWidth: true
            clip: true
            visible: SampleManager.downloadInProgress
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: !SampleManager.downloadInProgress
            spacing: 10

            Button {
                text: SampleManager.downloadFailed ? "Retry download all offline data" : "Download all offline data"
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
                clip: true
            }

            Button {
                text: qsTr("Delete all offline data")
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
                clip: true
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: !SampleManager.downloadInProgress

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                text: "Show only downloaded data"
                checked: showOnlyDownloaded
                enabled: {
                    if (SampleManager.offlineDataProjects.length === 0) return false;
                    for (let i = 0; i < SampleManager.offlineDataProjects.length; i++) {
                        if (SampleManager.offlineDataProjects[i].downloaded) return true;
                    }
                    return false;
                }
                Layout.alignment: Qt.AlignRight
                onClicked: {
                    showOnlyDownloaded = checked;
                }
                onEnabledChanged: {
                    if (!enabled) {
                        showOnlyDownloaded = false;
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !SampleManager.downloadInProgress
            color: "transparent"
            border {
                color: "lightgray"
                width: 1
            }
            radius: 5

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
                        height: 80
                        color: "lightgray"
                        radius: 5
                        border {
                            color: "darkgray"
                            width: 1
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10

                            Column {
                                Layout.fillWidth: true
                                spacing: 5

                                Label {
                                    text: modelData.sample.name
                                    font {
                                        family: fontFamily
                                        weight: Font.DemiBold
                                    }
                                    elide: Label.ElideRight
                                }

                                Label {
                                    text: modelData.downloaded ? "Downloaded" : "Not downloaded"
                                    color: modelData.downloaded ? "green" : "red"
                                    font {
                                        family: fontFamily
                                    }
                                }
                            }

                            Button {
                                text: "Download"
                                enabled: !modelData.downloaded && !SampleManager.downloadInProgress
                                onClicked: {
                                    SampleManager.downloadProjectData(modelData.sample.name);
                                }
                            }

                            Button {
                                text: "Delete"
                                enabled: modelData.downloaded
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

        Label {
            text: SampleManager.downloadText
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            horizontalAlignment: Label.AlignHCenter
            visible: SampleManager.downloadInProgress || SampleManager.downloadFailed
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

        Button {
            // PortalItem::fetchData does not have a cancel method so we can only clear the remaining items from the download queue
            text: SampleManager.cancelDownload ? "Cancelling remaining downloads..." : "Cancel remaining downloads"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            visible: SampleManager.downloadInProgress
            enabled: !SampleManager.cancelDownload
            onClicked: {
                SampleManager.cancelDownload = true;
            }
            clip: true
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

