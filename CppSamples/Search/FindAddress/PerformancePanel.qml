// Copyright 2026 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import PerformanceMonitor

Rectangle {
    id: root

    required property PerformanceMonitor monitor

    // label + right-aligned value, reused for each phase metric.
    component PhaseRow: RowLayout {
        id: row
        Layout.fillWidth: true
        property string label
        property string value
        spacing: 10

        Text {
            text: row.label
            color: "#aaaaaa"
            // font {
            //     family: Theme.fontUi
            //     pixelSize: 11
            // }
        }

        Item { Layout.fillWidth: true }

        Text {
            text: row.value
            color: "#cccccc"
            // font {
            //     family: Theme.fontMono
            //     pixelSize: 11
            // }
        }
    }

    implicitWidth: 300
    implicitHeight: Math.min(
                        contentColumn.implicitHeight + 24,
                        parent ? Math.max(parent.height - 24, 0) : contentColumn.implicitHeight + 24)
    color: "white"//Theme.panelBg
    radius: 5.0//Theme.radius
    border.color: "black"//Theme.border
    border.width: 1

    // Swallow events on the panel so they don't reach the view beneath.
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        hoverEnabled: true
        onWheel: (wheel) => wheel.accepted = true;
    }

    // FPS color: green near refresh rate, yellow >= 30, red below.
    function fpsColor(fps: real, hz: real): color {
        if (!fps || fps <= 0) {
            return "#5cb85c" //redTheme.textMuted;
        }

        const target = hz > 0 ? hz : 60;
        if (fps >= target - 5) {
            return "#5cb85c" //Theme.good;
        }
        if (fps >= 30) {
            return "#f0ad4e" //Theme.warn;
        }
        return "#d9534f"//Theme.bad;
    }

    ScrollView {
        id: scroller
        anchors {
            fill: parent
            margins: 12
        }
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            id: contentColumn
            width: scroller.availableWidth
            spacing: 8

            Text {
                text: "PERFORMANCE"
                color: "#aaaaaa"
                Layout.fillWidth: true
                font {
                    family: "Segoe UI, Helvetica, Arial, sans-serif"
                    pixelSize: 11
                    letterSpacing: 1.5
                    bold: true
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#3a3a3a" }

            RowLayout {
                spacing: 8
                Layout.fillWidth: true

                Text {
                    text: "App FPS"
                    color: "#aaaaaa"
                    font {
                        family: "Segoe UI, Helvetica, Arial, sans-serif"
                        pixelSize: 12
                    }
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: root.monitor ? root.monitor.fps.toFixed(1) : "—"
                    color: root.monitor ? root.fpsColor(root.monitor.fps, root.monitor.refreshRateHz) : Theme.textMuted
                    font {
                        family: "Consolas, Courier New, monospace"
                        pixelSize: 16
                        bold: true
                    }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#3a3a3a" }

            // Frame total + phase averages
            ColumnLayout {
                spacing: 4
                Layout.fillWidth: true

                RowLayout {
                    spacing: 8
                    Layout.fillWidth: true

                    Text {
                        text: "Frame"
                        color: "#aaaaaa"
                        font {
                            family: "Segoe UI, Helvetica, Arial, sans-serif"
                            pixelSize: 12
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: root.monitor ? root.monitor.frameTimeMs.toFixed(1) + " ms" : "—"
                        color: "#ffffff"
                        font {
                            family: "Segoe UI, Helvetica, Arial, sans-serif"
                            pixelSize: 13
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    spacing: 4
                    PhaseRow { label: "sync";   value: root.monitor ? root.monitor.syncMs.toFixed(2) + " ms" : "—" }
                    PhaseRow { label: "render"; value: root.monitor ? root.monitor.renderMs.toFixed(2) + " ms" : "—" }
                    PhaseRow { label: "swap";   value: root.monitor ? root.monitor.swapMs.toFixed(2) + " ms" : "—" }
                }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#3a3a3a" }

            Text {
                visible: root.monitor && root.monitor.refreshRateHz > 0
                text: root.monitor ? "target " + root.monitor.refreshRateHz.toFixed(0) + " Hz" : ""
                color: "#666666"
                Layout.fillWidth: true
                Layout.topMargin: 2
                font {
                    family: "Segoe UI, Helvetica, Arial, sans-serif"
                    pixelSize: 10
                }
            }
        }
    }
}
