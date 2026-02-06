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

Page {
    id: sampleListView
    visible: stackView.currentItem === this

    property string currentCategory: ""

    header: ToolBar {
        height: 42

        ToolButton {
            id: back
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: 5
            }
            icon.source: "qrc:/chevron-left.svg"
            icon.width: 32
            icon.height: 32
            icon.color: Calcite.text1
            flat: true

            background: Rectangle {
                implicitWidth: 36
                implicitHeight: 30
                radius: 5
                color: back.hovered ? Calcite.foreground3 : "transparent"

                Rectangle {
                    id: backFlash
                    anchors.fill: parent
                    radius: parent.radius
                    color: Calcite.foreground3
                    opacity: 0
                }
            }

            SequentialAnimation {
                id: backTapAnim
                ParallelAnimation {
                    NumberAnimation { target: back; property: "scale"; to: 0.92; duration: 80; easing.type: Easing.OutQuad }
                    NumberAnimation { target: backFlash; property: "opacity"; to: 1; duration: 80 }
                }
                ParallelAnimation {
                    NumberAnimation { target: back; property: "scale"; to: 1.0; duration: 200; easing.type: Easing.OutBack }
                    NumberAnimation { target: backFlash; property: "opacity"; to: 0; duration: 250; easing.type: Easing.OutQuad }
                }
            }

            onClicked: {
                backTapAnim.restart()
                stackView.pop()
            }
        }

        Label {
            anchors.centerIn: parent
            text: currentCategory
            font {
                pixelSize: 18
                family: fontFamily
            }
            color: Calcite.text1
        }
    }

    Rectangle {
        id: sampleListRect
        anchors {
            fill: parent
            margins: 10
        }
        color: "transparent"

        property var activeContextMenu: null

        ListView {
            id: samplesListView
            anchors.fill: parent
            clip: true
            model: SampleManager.currentCategory ? SampleManager.currentCategory.samples : []
            spacing: 0
            delegate: ItemDelegate {
                id: itemDelegate
                width: parent ? parent.width : 0
                height: Math.max(40, contentLabel.implicitHeight + 16)
                clip: true
                text: name
                enabled: !sampleListRect.activeContextMenu || contextMenu.visible
                opacity: (sampleListRect.activeContextMenu !== null && !contextMenu.visible) ? 0.3 : 1.0

                Behavior on opacity { NumberAnimation { duration: 150 } }

                property bool isFavorite: false
                property bool isSelected: SampleManager.currentSample === sample
                property bool menuJustClosed: false

                Connections {
                    target: contextMenu
                    function onClosed() {
                        if (itemDelegate.hovered) {
                            itemDelegate.menuJustClosed = true;
                        }
                    }
                }

                onHoveredChanged: {
                    // Reset flag when mouse leaves
                    if (!hovered) {
                        menuJustClosed = false;
                    }
                }

                TapHandler {
                    id: tapHandler
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
                        // Left click / tap
                        if (itemDelegate.isSelected) return;
                        if (itemDelegate.menuJustClosed) {
                            itemDelegate.menuJustClosed = false;
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
                        text: itemDelegate.text
                        font: itemDelegate.font
                        color: itemDelegate.isSelected ? "#ffffff" : Calcite.text1
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
                            id: starButton
                            width: 18
                            height: 18
                            visible: itemDelegate.isFavorite
                            background: Item {}
                            icon.source: "qrc:/star-f.svg"
                            icon.color: "#FFD700"
                            icon.width: 18
                            icon.height: 18
                            padding: 0
                        }

                        Rectangle {
                            id: kebabButton
                            width: 24
                            height: 24
                            radius: 4
                            enabled: ((itemDelegate.hovered && itemDelegate.enabled) || contextMenu.visible) ? 1 : 0
                            opacity: ((itemDelegate.hovered && itemDelegate.enabled) || contextMenu.visible) ? 1 : 0
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
                                    colorizationColor: itemDelegate.isSelected ? "#ffffff"
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
                        opacity: ((itemDelegate.hovered && itemDelegate.enabled) || contextMenu.visible) ? 1 : 0
                        visible: !itemDelegate.isSelected

                        Behavior on opacity {
                            enabled: !itemDelegate.hovered
                            NumberAnimation { duration: 150 }
                        }
                    }

                    // Selected highlight
                    Rectangle {
                        anchors.fill: parent
                        color: Calcite.brandHover
                        visible: itemDelegate.isSelected
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
                        itemDelegate.isFavorite = SampleManager.isFavorite(sample);
                    }
                }

                Component.onCompleted: {
                    isFavorite = SampleManager.isFavorite(sample);
                }

                Menu {
                    id: contextMenu

                    onOpened: sampleListRect.activeContextMenu = contextMenu
                    onClosed: sampleListRect.activeContextMenu = null

                    background: Rectangle {
                        implicitWidth: 200
                        color: Calcite.foreground1
                        border.color: Calcite.border1
                        radius: 4
                    }

                    MenuItem {
                        text: qsTr("Live Sample")
                        enabled: {
                            if(SampleManager.currentSample == sample) {
                                return (SampleManager.currentMode != SampleManager.LiveSampleView);
                            }
                            else {
                                return true;
                            }
                        }
                        onTriggered: {
                            drawer.close();
                            if(SampleManager.currentSample != sample) {
                                SampleManager.currentSample = sample;
                            }
                            SampleManager.currentMode = SampleManager.LiveSampleView;
                        }
                    }

                    MenuItem {
                        text: qsTr("Source Code")
                        enabled: {
                            if(SampleManager.currentSample == sample) {
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
                            if(SampleManager.currentSample == sample) {
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
                        text: itemDelegate.isFavorite ? qsTr("Remove from Favorites") : qsTr("Add to Favorites")
                        onTriggered: {
                            const styledName = "<font color='" + Calcite.brand + "'>" + name + "</font>";
                            if (itemDelegate.isFavorite) {
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
        }

        // Click-blocking overlay when context menu is open
        MouseArea {
            anchors.fill: parent
            visible: sampleListRect.activeContextMenu !== null
            z: 99
            onClicked: {
                if (sampleListRect.activeContextMenu) {
                    sampleListRect.activeContextMenu.close();
                }
            }
        }
    }
}
