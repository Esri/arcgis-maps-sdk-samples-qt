// [Legal]
// Copyright 2025 Esri.

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
import QtQuick.Effects
import Esri.ArcGISRuntimeSamples
import Calcite
import Qt5Compat.GraphicalEffects

ColumnLayout {
    id: root

    required property real scaleFactor
    required property real baseMargin
    required property real baseSpacing
    required property real baseFontSize
    required property var deleteProjectDialog
    required property var deleteAllDialog

    property bool showOnlyDownloaded: false
    property bool hasDataToDelete: false

    anchors.fill: parent
    anchors.margins: baseMargin
    spacing: baseSpacing

    function refreshState() {
        root.hasDataToDelete = SampleManager.hasAnyDataToDelete();
        if (!root.hasDataToDelete && root.showOnlyDownloaded) {
            root.showOnlyDownloaded = false;
        }
    }

    Connections {
        target: SampleManager.downloadsManager
        function onOfflineDataStateChanged() {
            root.refreshState();
        }
    }

    Component.onCompleted: {
        root.refreshState();
    }

    onVisibleChanged: {
        if (visible) {
            root.refreshState();
        }
    }

    // Logo
    Image {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        source: "qrc:/logo.png"
        Layout.preferredWidth: Math.max(80, 125 * scaleFactor)
        Layout.preferredHeight: Math.max(80, 125 * scaleFactor)
        fillMode: Image.PreserveAspectFit
        clip: true
    }

    // Download all / Delete all
    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        spacing: baseSpacing

        Button {
            id: downloadAllButton
            text: SampleManager.downloadsManager.downloadFailed ? qsTr("Retry download all offline data") : qsTr("Download all offline data")
            Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
            Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
            enabled: SampleManager.hasAnyDataToDownload
            font.pixelSize: Math.max(10, baseFontSize)
            clip: true

            background: Rectangle {
                radius: 4
                color: downloadAllButton.down ? Qt.darker(Calcite.brand, 1.2) : (downloadAllButton.hovered ? Calcite.brandHover : Calcite.brand)
            }

            onClicked: {
                if (SampleManager.reachability === SampleManager.ReachabilityOnline || SampleManager.reachability === SampleManager.ReachabilityUnknown) {
                    SampleManager.downloadAllDataItems();
                } else {
                    SampleManager.currentMode = SampleManager.NetworkRequiredView;
                }
            }
        }

        Button {
            id: deleteAllButton
            text: qsTr("Delete all offline data")
            Layout.preferredWidth: Math.max(implicitWidth, 200 * scaleFactor)
            Layout.preferredHeight: Math.max(40, 50 * scaleFactor)
            visible: root.hasDataToDelete
            font.pixelSize: Math.max(10, baseFontSize)
            clip: true

            background: Rectangle {
                radius: 4
                color: deleteAllButton.down ? Qt.darker(Calcite.brand, 1.2) : (deleteAllButton.hovered ? Calcite.brandHover : Calcite.brand)
            }

            onClicked: {
                deleteAllDialog.resetDialog();
                deleteAllDialog.visible = true;
            }
        }
    }

    // filter checkbox
    RowLayout {
        Layout.fillWidth: true
        Layout.topMargin: baseSpacing * 1.25
        Layout.bottomMargin: baseSpacing * 1.25
        Layout.leftMargin: baseMargin
        Layout.rightMargin: baseMargin
        spacing: baseSpacing

        Item {
            Layout.fillWidth: true
            visible: true
        }

        CheckBox {
            text: qsTr("Show only downloaded data")
            checked: root.showOnlyDownloaded
            enabled: root.hasDataToDelete
            onClicked: {
                root.showOnlyDownloaded = checked;
            }
            font.pixelSize: Math.max(10, baseFontSize)
        }
    }

    // Samples list
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
            ScrollBar.vertical.policy: ScrollBar.AsNeeded
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                model: SampleManager.downloadsManager.offlineDataProjects
                spacing: 0

                Component.onCompleted: {
                    SampleManager.getOfflineDataProjects();
                }

                // Individual sample item
                delegate: Item {
                    width: ListView.view.width
                    height: (!root.showOnlyDownloaded || model.downloaded) ? Math.max(65, 80 * root.scaleFactor) + 5 : 0

                    Rectangle {
                        anchors.fill: parent
                        anchors.bottomMargin: 5
                        visible: !root.showOnlyDownloaded || model.downloaded
                        color: Calcite.foreground2
                        radius: 8
                        border {
                            color: Calcite.border2
                            width: 1
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors {
                                margins: root.baseMargin
                                topMargin: 2
                                bottomMargin: 2
                            }
                            spacing: root.baseSpacing / 2

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                spacing: root.baseSpacing

                                Column {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    // Sample name
                                    Label {
                                        text: model.sample.name
                                        font {
                                            family: fontFamily
                                            weight: Font.DemiBold
                                            pixelSize: Math.max(11, root.baseFontSize + 1)
                                        }
                                        elide: Label.ElideRight
                                        width: parent.width
                                        wrapMode: Text.WordWrap
                                        maximumLineCount: 2
                                    }

                                    // Status badge (Downloaded/Downloading/Partial/Not downloaded)
                                    Rectangle {
                                        width: statusLabel.implicitWidth + 20
                                        height: statusLabel.implicitHeight + 10
                                        color: {
                                            if (model.downloading) {
                                                return Calcite.warning;
                                            } else if (model.downloaded) {
                                                return Calcite.success;
                                            } else if (model.downloadedItemsCount > 0 && model.downloadedItemsCount < model.totalItemsCount) {
                                                return "#FFA500";
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
                                                pixelSize: Math.max(9, root.baseFontSize - 2)
                                                weight: Font.Medium
                                            }
                                        }
                                    }

                                    // Download progress bar (shown during download)
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

                                // Action buttons: Download / Cancel / Delete
                                RowLayout {
                                    spacing: root.baseSpacing / 2
                                    property bool useIconMode: root.scaleFactor < 0.7
                                    readonly property real buttonSize: Math.max(32, 40 * root.scaleFactor)
                                    readonly property real iconSize: buttonSize * 0.7

                                    Button {
                                        id: downloadButton
                                        Layout.preferredWidth: parent.useIconMode ? parent.buttonSize : Math.max(110, 110 * root.scaleFactor)
                                        Layout.preferredHeight: parent.buttonSize
                                        visible: model.downloadedItemsCount < model.totalItemsCount && !model.downloading
                                        padding: 0

                                        background: Rectangle {
                                            radius: downloadButton.parent.useIconMode ? downloadButton.width / 2 : 4
                                            color: downloadButton.down ? Qt.darker(Calcite.brand, 1.2) : (downloadButton.hovered ? Calcite.brandHover : Calcite.brand)
                                        }

                                        contentItem: Item {
                                            Image {
                                                id: downloadIcon
                                                source: "qrc:/download.svg"
                                                width: downloadButton.parent.iconSize
                                                height: downloadButton.parent.iconSize
                                                sourceSize.width: downloadButton.parent.iconSize
                                                sourceSize.height: downloadButton.parent.iconSize
                                                fillMode: Image.PreserveAspectFit
                                                anchors.centerIn: parent
                                                visible: downloadButton.parent.useIconMode
                                                layer.enabled: true
                                                layer.smooth: true
                                                layer.effect: ColorOverlay {
                                                    color: Calcite.offWhite
                                                }
                                            }

                                            Label {
                                                text: qsTr("Download")
                                                font.pixelSize: Math.max(12, root.baseFontSize - 1)
                                                color: Calcite.offWhite
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                                anchors.fill: parent
                                                visible: !downloadButton.parent.useIconMode
                                            }
                                        }

                                        onClicked: {
                                            SampleManager.downloadProjectData(model.sample.name);
                                        }
                                    }

                                    Button {
                                        id: deleteButton
                                        Layout.preferredWidth: parent.useIconMode ? parent.buttonSize : Math.max(110, 110 * root.scaleFactor)
                                        Layout.preferredHeight: parent.buttonSize
                                        visible: model.downloadedItemsCount > 0 && !model.downloading
                                        padding: 0

                                        background: Rectangle {
                                            radius: deleteButton.parent.useIconMode ? deleteButton.width / 2 : 4
                                            color: deleteButton.down ? Qt.darker(Calcite.brand, 1.2) : (deleteButton.hovered ? Calcite.brandHover : Calcite.brand)
                                        }

                                        contentItem: Item {
                                            Image {
                                                id: deleteIcon
                                                source: "qrc:/trash.svg"
                                                width: deleteButton.parent.iconSize
                                                height: deleteButton.parent.iconSize
                                                sourceSize.width: deleteButton.parent.iconSize
                                                sourceSize.height: deleteButton.parent.iconSize
                                                fillMode: Image.PreserveAspectFit
                                                anchors.centerIn: parent
                                                visible: deleteButton.parent.useIconMode
                                                layer.enabled: true
                                                layer.smooth: true
                                                layer.effect: ColorOverlay {
                                                    color: Calcite.offWhite
                                                }
                                            }

                                            Label {
                                                text: qsTr("Delete")
                                                font.pixelSize: Math.max(12, root.baseFontSize - 1)
                                                color: Calcite.offWhite
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                                anchors.fill: parent
                                                visible: !deleteButton.parent.useIconMode
                                            }
                                        }

                                        onClicked: {
                                            root.deleteProjectDialog.sampleName = model.sample.name.toString();
                                            root.deleteProjectDialog.resetDialog();
                                            root.deleteProjectDialog.visible = true;
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
