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

pragma ComponentBehavior: Bound

import QtQuick

Item {
    id: root

    // perfMonitor is a context property set by PerformanceMonitor::install()
    readonly property var monitor: perfMonitor

    property bool expanded: false

    // Recomputed at open and close: growth heads toward the window center, collapse heads away from it.
    property bool expandLeft: false
    property bool expandUp: false

    readonly property real targetFps: monitor.refreshRateHz > 0 ? monitor.refreshRateHz : 60
    readonly property real rawFps: monitor.frameTimeMs > 0 ? 1000 / monitor.frameTimeMs : 0

    readonly property real fps: Math.min(rawFps, targetFps)

    readonly property bool idle: monitor.frameTimeMs <= 0

    readonly property color fpsColor: {
        if (idle)
        return mutedColor;
        if (!monitor.forceRender)
        return neutralColor;
        return fps >= targetFps - 5 ? goodColor : fps >= 30 ? warnColor : badColor;
    }

    readonly property color goodColor: "#3BB273"
    readonly property color warnColor: "#E5B301"
    readonly property color badColor: "#D6503C"
    readonly property color mutedColor: "#8C8C8C"
    readonly property color neutralColor: "#DDDDDD"

    width: 78
    height: 30

    Component.onCompleted: {
        x = 16;
        y = 50;
    }

    Connections {
        target: root.parent

        function onWidthChanged() { const w = root.expandLeft ? root.width : panel.width; root.x = Math.min(root.x, Math.max(0, root.parent.width - w - 8)) }
        function onHeightChanged() { const h = root.expandUp ? root.height : panel.height; root.y = Math.min(root.y, Math.max(0, root.parent.height - h - 8)) }
    }

    function ms(value: real): string {
        if (value <= 0)
            return "—";
        if (value >= 1000)
            return qsTr("%1 s").arg((value / 1000).toFixed(2));
        return qsTr("%1 ms").arg(value.toFixed(1));
    }

    function reanchorForClose() {
        const px = x + panel.x;
        const py = y + panel.y;
        expandLeft = px + panel.width / 2 > parent.width / 2;
        expandUp = py + panel.height / 2 > parent.height / 2;
        x = Math.min(Math.max(0, expandLeft ? px + panel.width - width : px), parent.width - width);
        y = Math.min(Math.max(0, expandUp ? py + panel.height - height : py), parent.height - height);
    }

    component MetricRow: Item {
        id: metricRow

        required property string label
        required property string valueText
        property color valueColor: root.neutralColor

        width: parent ? parent.width : 0
        height: 16

        Text {
            anchors {
                left: parent.left
                right: valueItem.left
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }
            text: metricRow.label
            textFormat: Text.PlainText
            elide: Text.ElideRight
            color: "#8C8C8C"
            font.pixelSize: 11
        }

        Text {
            id: valueItem
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

    MouseArea {
        anchors.fill: panel
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true

        drag {
            target: root
            threshold: 8
            minimumX: root.expandLeft ? panel.width - root.width : 0
            maximumX: root.parent ? Math.max(0, root.parent.width - (root.expandLeft ? root.width : panel.width)) : 0
            minimumY: root.expandUp ? panel.height - root.height : 0
            maximumY: root.parent ? Math.max(0, root.parent.height - (root.expandUp ? root.height : panel.height)) : 0
        }

        onClicked: mouse => {
            if (mouse.button !== Qt.LeftButton)
            return;
            if (root.parent) {
                if (root.expanded) {
                    root.reanchorForClose();
                } else {
                    root.expandLeft = root.x + root.width / 2 > root.parent.width / 2;
                    root.expandUp = root.y + root.height / 2 > root.parent.height / 2;
                }
            }
            root.expanded = !root.expanded;
        }
        onDoubleClicked: mouse => mouse.accepted = true
        onWheel: wheel => wheel.accepted = true
    }

    Rectangle {
        id: panel

        PinchHandler { target: null }

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

        Text {
            anchors.centerIn: parent
            visible: !root.expanded
            text: root.idle ? qsTr("idle") : qsTr("%1 fps").arg(root.fps.toFixed(0))
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
                text: qsTr("PERFORMANCE")
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
                label: qsTr("FPS")
                valueText: root.idle ? qsTr("idle") : root.fps.toFixed(1)
                valueColor: root.fpsColor
            }

            MetricRow { label: qsTr("frame"); valueText: root.ms(root.monitor.frameTimeMs) }

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
                    text: qsTr("force render")
                    color: root.mutedColor
                    font.pixelSize: 11
                }

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
                        anchors.margins: -6
                        onClicked: root.monitor.forceRender = !root.monitor.forceRender
                    }
                }
            }

            Text {
                text: root.monitor.refreshRateHz > 0
                      ? qsTr("target %1 Hz").arg(root.monitor.refreshRateHz.toFixed(0))
                      : ""
                visible: root.monitor.refreshRateHz > 0
                color: "#6E6E6E"
                font.pixelSize: 10
            }
        }
    }

}
