// [Legal]
// Copyright 2026 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

// Draggable performance HUD, collapsed to a pill by default; colors are hardcoded so the module carries no toolkit dependency.

pragma ComponentBehavior: Bound

import QtQuick

Item {
    id: root

    // perfMonitor is a context property set by PerformanceMonitor::install()
    readonly property var monitor: perfMonitor

    property bool expanded: false

    // Locked at expand time so the panel grows toward the window center and collapses back onto the pill.
    property bool expandLeft: false
    property bool expandUp: false

    readonly property real fps: monitor.frameTimeMs > 0 ? 1000 / monitor.frameTimeMs : 0
    readonly property bool idle: monitor.frameTimeMs <= 0
    readonly property real targetFps: monitor.refreshRateHz > 0 ? monitor.refreshRateHz : 60
    readonly property color fpsColor: idle ? mutedColor : fps >= targetFps - 5 ? goodColor : fps >= 30 ? warnColor : badColor

    readonly property color goodColor: "#3BB273"
    readonly property color warnColor: "#E5B301"
    readonly property color badColor: "#D6503C"
    readonly property color mutedColor: "#8C8C8C"

    // The root stays pill-sized; the panel expands out of it via anchors, so x/y never move on expand/collapse.
    width: 78
    height: 30

    // Set once, not bound (a binding would fight the drag); top-left, below the 42px header ToolBar.
    Component.onCompleted: {
        x = 16;
        y = 50;
    }

    Connections {
        target: root.parent

        function onWidthChanged() { root.x = Math.min(root.x, Math.max(0, root.parent.width - root.width - 8)) }
        function onHeightChanged() { root.y = Math.min(root.y, Math.max(0, root.parent.height - root.height - 8)) }
    }

    function ms(value: real): string {
        return value > 0 ? value.toFixed(1) + " ms" : "—";
    }

    // label + right-aligned value. Self-contained so it needs no ids from the enclosing scope.
    component MetricRow: Item {
        id: metricRow

        // Not named "value": that name is reserved for auto-filling from the metrics model role.
        required property string label
        required property string valueText
        property color valueColor: "#DDDDDD"

        width: parent ? parent.width : 0
        height: 16

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: metricRow.label
            textFormat: Text.PlainText
            color: "#8C8C8C"
            font.pixelSize: 11
        }

        Text {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: metricRow.valueText
            textFormat: Text.PlainText
            color: metricRow.valueColor

            font {
                pixelSize: 11
                family: "Courier New"
                preferShaping: false
            }
        }
    }

    // Declared before the panel so later siblings (the toggle) get first refusal on a press; misses fall through to drag/click here.
    MouseArea {
        anchors.fill: panel
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        drag {
            target: root
            threshold: 8
            minimumX: 0
            maximumX: root.parent ? Math.max(0, root.parent.width - root.width) : 0
            minimumY: 0
            maximumY: root.parent ? Math.max(0, root.parent.height - root.height) : 0
        }

        onClicked: mouse => {
            if (mouse.button !== Qt.LeftButton)
                return;
            if (!root.expanded && root.parent) {
                root.expandLeft = root.x + root.width / 2 > root.parent.width / 2;
                root.expandUp = root.y + root.height / 2 > root.parent.height / 2;
            }
            root.expanded = !root.expanded;
        }
        onDoubleClicked: mouse => mouse.accepted = true
        onWheel: wheel => wheel.accepted = true
    }

    Rectangle {
        id: panel

        // Grabs two-finger gestures so a pinch starting on the HUD cannot zoom the map beneath it.
        PinchHandler { target: null }

        // The anchored edge is the one nearest the window corner, so growth heads toward the center.
        anchors {
            left: root.expandLeft ? undefined : root.left
            right: root.expandLeft ? root.right : undefined
            top: root.expandUp ? undefined : root.top
            bottom: root.expandUp ? root.bottom : undefined
        }
        width: root.expanded ? 186 : root.width
        height: root.expanded ? content.implicitHeight + 20 : root.height
        radius: 6
        color: "#D91A1A1A"
        border.color: "#33FFFFFF"
        border.width: 1

        Behavior on width { NumberAnimation { duration: 120; easing.type: Easing.OutQuad } }
        Behavior on height { NumberAnimation { duration: 120; easing.type: Easing.OutQuad } }

        // Collapsed: just the headline number.
        Text {
            anchors.centerIn: parent
            visible: !root.expanded
            text: root.idle ? "idle" : root.fps.toFixed(0) + " fps"
            textFormat: Text.PlainText
            color: root.fpsColor

            font {
                pixelSize: 13
                bold: true
                family: "Courier New"
                preferShaping: false
            }
        }

        Column {
            id: content

            anchors.fill: parent
            anchors.margins: 10
            visible: root.expanded
            spacing: 5
            clip: true

            Text {
                text: "PERFORMANCE"
                textFormat: Text.PlainText
                color: "#FFFFFF"

                font {
                    pixelSize: 10
                    bold: true
                    letterSpacing: 1.2
                }
            }

            Rectangle { width: content.width; height: 1; color: "#33FFFFFF" }

            MetricRow {
                label: "FPS"
                valueText: root.idle ? "idle" : root.fps.toFixed(1)
                valueColor: root.fpsColor
            }

            MetricRow { label: "frame"; valueText: root.ms(root.monitor.frameTimeMs) }

            // One row per app-reported metric, in first-report order; unbound while collapsed so hidden rows cost nothing.
            Repeater {
                model: root.expanded ? root.monitor.metrics : null

                delegate: MetricRow {
                    // label auto-fills from the model role of the same name.
                    required property double value
                    valueText: root.ms(value)
                }
            }

            Rectangle { width: content.width; height: 1; color: "#33FFFFFF" }

            Item {
                width: content.width
                height: 18

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "force render"
                    color: root.mutedColor
                    font.pixelSize: 11
                }

                // A plain rectangle rather than a Controls Switch, so the viewer's Calcite styling cannot reach this module.
                Rectangle {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 28
                    height: 14
                    radius: 7
                    color: root.monitor.forceRender ? root.goodColor : "#4D4D4D"

                    Rectangle {
                        width: 10
                        height: 10
                        radius: 5
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        x: root.monitor.forceRender ? parent.width - width - 2 : 2

                        Behavior on x { NumberAnimation { duration: 100 } }
                    }

                    MouseArea {
                        anchors.fill: parent
                        // Negative margins enlarge the hit target for touch.
                        anchors.margins: -6
                        onClicked: root.monitor.forceRender = !root.monitor.forceRender
                    }
                }
            }

            Text {
                text: root.monitor.refreshRateHz > 0
                      ? "target " + root.monitor.refreshRateHz.toFixed(0) + " Hz"
                      : ""
                visible: root.monitor.refreshRateHz > 0
                color: "#6E6E6E"
                font.pixelSize: 10
            }
        }
    }

}
