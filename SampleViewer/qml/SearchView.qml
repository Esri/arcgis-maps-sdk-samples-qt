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
import QtQuick.Effects
import Esri.ArcGISRuntimeSamples

ListView {
    spacing: 0
    clip: true

    property var activeContextMenu: null

    delegate: ItemDelegate {
        id: searchDelegate
        width: parent ? parent.width : 0
        height: Math.max(40, contentLabel.implicitHeight + 16)
        clip: true
        text: name
        enabled: !listView.activeContextMenu || contextMenu.visible
        opacity: (listView.activeContextMenu !== null && !contextMenu.visible) ? 0.3 : 1.0

        Behavior on opacity { NumberAnimation { duration: 150 } }

        property bool isFavorite: false
        property bool isSelected: SampleManager.currentSample === sample
        property bool menuJustClosed: false

        // Grab reference to the parent ListView
        property ListView listView: ListView.view

        Connections {
            target: contextMenu
            function onClosed() {
                if (searchDelegate.hovered) {
                    searchDelegate.menuJustClosed = true;
                }
            }
        }

        onHoveredChanged: {
            if (!hovered) {
                menuJustClosed = false;
            }
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            property bool didLongPress: false

            onLongPressed: {
                didLongPress = true;
                contextMenu.popup(point.position.x, point.position.y)
            }

            onTapped: function(eventPoint, button) {
                if (didLongPress) {
                    didLongPress = false;
                    return;
                }
                if (button === Qt.RightButton) {
                    contextMenu.popup()
                    return;
                }
                if (searchDelegate.isSelected) return;
                if (searchDelegate.menuJustClosed) {
                    searchDelegate.menuJustClosed = false;
                    return;
                }
                drawer.close();
                SampleManager.currentSample = sample;
                SampleManager.currentMode = SampleManager.LiveSampleView;
            }

            onCanceled: didLongPress = false
        }

        contentItem: Item {
            Label {
                id: contentLabel
                anchors {
                    left: parent.left
                    right: rightIcons.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: 8
                    rightMargin: 8
                }
                text: searchDelegate.text
                font: searchDelegate.font
                color: searchDelegate.isSelected ? "#ffffff" : Calcite.text1
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }

            Row {
                id: rightIcons
                anchors {
                    right: parent.right
                    rightMargin: 8
                    verticalCenter: parent.verticalCenter
                }
                spacing: 4

                ToolButton {
                    width: 18
                    height: 18
                    visible: searchDelegate.isFavorite
                    background: Item {}
                    icon {
                        source: "qrc:/star-f.svg"
                        color: "#FFD700"
                        width: 18
                        height: 18
                    }
                    padding: 0
                }

                Rectangle {
                    id: kebabButton
                    width: 24
                    height: 24
                    radius: 4
                    enabled: ((searchDelegate.hovered && searchDelegate.enabled) || contextMenu.visible) ? 1 : 0
                    opacity: ((searchDelegate.hovered && searchDelegate.enabled) || contextMenu.visible) ? 1 : 0
                    color: "transparent"

                    Behavior on opacity { NumberAnimation { duration: 150 } }

                    property bool menuJustClosed: false

                    Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: Calcite.foreground1
                        opacity: kebabMouseArea.containsMouse ? 1 : 0

                        Behavior on opacity {
                            NumberAnimation { duration: 150 }
                        }
                    }

                    Image {
                        id: kebabIcon
                        anchors.centerIn: parent
                        source: "qrc:/ellipsis.svg"
                        width: 18
                        height: 18
                        sourceSize: Qt.size(18, 18)
                        visible: true

                        layer.enabled: true
                        layer.effect: MultiEffect {
                            anchors.fill: kebabIcon
                            source: kebabIcon
                            colorization: 1.0
                            brightness: 1.0
                            colorizationColor: searchDelegate.isSelected ? "#ffffff"
                                                                         : kebabMouseArea.containsMouse ? Calcite.text1 : Calcite.text2
                        }
                    }

                    MouseArea {
                        id: kebabMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (kebabButton.menuJustClosed) {
                                kebabButton.menuJustClosed = false;
                                return;
                            }
                            contextMenu.popup();
                        }
                    }

                    Connections {
                        target: contextMenu
                        function onClosed() {
                            if (kebabMouseArea.containsMouse) {
                                kebabButton.menuJustClosed = true;
                            }
                        }
                    }
                }
            }
        }

        background: Rectangle {
            width: parent.width
            height: parent.height
            color: "transparent"

            // Hover highlight
            Rectangle {
                anchors.fill: parent
                color: Calcite.foreground3
                opacity: ((searchDelegate.hovered && searchDelegate.enabled) || contextMenu.visible) ? 1 : 0
                visible: !searchDelegate.isSelected

                Behavior on opacity {
                    enabled: !searchDelegate.hovered
                    NumberAnimation { duration: 150 }
                }
            }

            // Selected highlight
            Rectangle {
                anchors.fill: parent
                color: Calcite.brandHover
                visible: searchDelegate.isSelected
            }

            // Bottom separator line
            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: 1
                color: Calcite.border1
            }
        }

        Connections {
            target: SampleManager
            function onFavoriteSamplesChanged() {
                searchDelegate.isFavorite = SampleManager.isFavorite(sample);
            }
        }

        Component.onCompleted: {
            isFavorite = SampleManager.isFavorite(sample);
        }

        Menu {
            id: contextMenu

            onOpened: searchDelegate.listView.activeContextMenu = contextMenu
            onClosed: searchDelegate.listView.activeContextMenu = null

            background: Rectangle {
                implicitWidth: 200
                color: Calcite.foreground1
                border.color: Calcite.border1
                radius: 4
            }

            MenuItem {
                text: qsTr("Live Sample")
                enabled: {
                    if (SampleManager.currentSample == sample) {
                        return (SampleManager.currentMode != SampleManager.LiveSampleView);
                    }
                    else {
                        return true;
                    }
                }
                onTriggered: {
                    drawer.close();
                    if (SampleManager.currentSample != sample) {
                        SampleManager.currentSample = sample;
                    }
                    SampleManager.currentMode = SampleManager.LiveSampleView;
                }
            }

            MenuItem {
                text: qsTr("Source Code")
                enabled: {
                    if (SampleManager.currentSample == sample) {
                        return (SampleManager.currentMode != SampleManager.SourceCodeView);
                    }
                    else {
                        return true;
                    }
                }
                onTriggered: {
                    drawer.close();
                    SampleManager.currentSample = sample;
                    SampleManager.currentMode = SampleManager.SourceCodeView;
                }
            }

            MenuItem {
                text: qsTr("Description")
                enabled: {
                    if (SampleManager.currentSample == sample) {
                        return (SampleManager.currentMode != SampleManager.DescriptionView);
                    }
                    else {
                        return true;
                    }
                }
                onTriggered: {
                    drawer.close();
                    SampleManager.currentSample = sample;
                    SampleManager.currentMode = SampleManager.DescriptionView;
                }
            }

            MenuItem {
                text: searchDelegate.isFavorite ? qsTr("Remove from Favorites") : qsTr("Add to Favorites")
                onTriggered: {
                    const styledName = "<font color='" + Calcite.brand + "'>" + name + "</font>";
                    if (searchDelegate.isFavorite) {
                        SampleManager.removeSampleFromFavorites(sample);
                        window.showToast(qsTr("Removed %1 from favorites").arg(styledName));
                    } else {
                        SampleManager.addSampleToFavorites(sample);
                        window.showToast(qsTr("Added %1 to favorites").arg(styledName));
                    }
                }
            }
        }
    }

    // Click-blocking overlay when context menu is open
    MouseArea {
        anchors.fill: parent
        visible: activeContextMenu !== null
        z: 99
        onClicked: {
            if (activeContextMenu) {
                activeContextMenu.close();
            }
        }
    }
}
