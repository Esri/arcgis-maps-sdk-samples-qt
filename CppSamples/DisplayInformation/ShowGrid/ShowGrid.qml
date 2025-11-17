// [WriteFile Name=ShowGrid, Category=DisplayInformation]
// [Legal]
// Copyright 2024 Esri.
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esri.Samples

Item {
    id: root
    property real centerWindowY: (root.height / 2) - (styleWindow.height / 2)

    MapView {
        id: mapQuickView
        anchors.fill: parent
        visible: gridSample.currentViewType == "MapView"
        Component.onCompleted: {
            // Set and keep the focus on SceneView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    SceneView {
        id: sceneQuickView
        anchors.fill: parent
        visible: gridSample.currentViewType == "SceneView"
    }

    // Declare the C++ instance which creates the map, scene etc. and supply the views
    ShowGridSample {
        id: gridSample
        mapView: mapQuickView
        sceneView: sceneQuickView

        onCurrentViewTypeChanged: {
            if (gridSample.currentViewType === "MapView")
                mapQuickView.forceActiveFocus();
            else
                sceneQuickView.forceActiveFocus();
        }
    }

    Row {
        id: viewButtonsRow
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 5
        }

        spacing: 2

        Rectangle {
            id: mapViewButton
            property bool selected: gridSample.currentViewType === "MapView"

            width: 100
            height: 30

            color: selected ? palette.highlight : palette.base
            radius: 8
            border {
                color: selected ? palette.base : palette.highlight
                width: selected ? 2 : 1
            }

            Label {
                anchors.centerIn: parent
                text: qsTr("Map View")
                font.pixelSize: 14
                color: mapViewButton.selected ? "white" : palette.text
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    gridSample.setViewType("MapView");
                    positionCombo.enabled = true;
                }
            }
        }

        Rectangle {
            id: sceneViewButton
            property bool selected: gridSample.currentViewType === "SceneView"

            width: 100
            height: 30

            color: selected ? palette.highlight : palette.base
            radius: 8
            border {
                color: selected ? palette.base : palette.highlight
                width: selected ? 2 : 1
            }

            Label {
                anchors.centerIn: parent
                text: qsTr("Scene View")
                font.pixelSize: 14
                color: sceneViewButton.selected ? "white" : palette.text
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    gridSample.setViewType("SceneView");
                    if (gridTypeComboBox.currentText !== "LatLong") {
                        positionCombo.currentIndex = 0;
                        positionCombo.enabled = false;
                    } else {
                        positionCombo.enabled = true;
                    }
                }
            }
        }
    }

    // Button to view the styling window
    Button {
        id: styleButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }

        width: 150
        height: 30
        text: qsTr("Change grid style")
        font.pixelSize: 14

        onClicked: {
            background.visible = true;
            showAnimation.restart();
        }
    }

    // background fade rectangle
    Rectangle {
        id: background
        anchors.fill: parent
        color: "gray"
        opacity: 0.7
        visible: false
        border {
            width: 0.5
            color: "black"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true;
        }
    }

    Rectangle {
        id: styleWindow
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        y: parent.height // initially display below the page
        height: childrenRect.height + 16
        width: childrenRect.width + 16

        color: palette.base
        radius: 4
        border {
            color: "black"
            width: 1
        }

        SequentialAnimation {
            id: showAnimation
            NumberAnimation { target: styleWindow; property: "y"; to: centerWindowY; duration: 200 }
        }

        SequentialAnimation {
            id: hideAnimation
            NumberAnimation { target: styleWindow; property: "y"; to: root.height; duration: 200 }
        }

        GridLayout {
            id: grid
            columns: 2
            anchors.centerIn: parent

            Label {
                Layout.leftMargin: 10
                text: qsTr("Grid type")
            }

            ComboBox {
                id: gridTypeComboBox
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: gridSample.gridTypes
                onCurrentTextChanged: {
                    gridSample.setGridType(currentText)
                    if (currentText !== "LatLong" && gridSample.currentViewType === "SceneView") {
                        positionCombo.currentIndex = 0;
                        positionCombo.enabled = false;
                    } else {
                        positionCombo.enabled = true;
                    }
                }
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        metricsGridTypeComboBox.text = model[i];
                        modelWidth = Math.max(modelWidth, metricsGridTypeComboBox.width);
                    }
                }
                TextMetrics {
                    id: metricsGridTypeComboBox
                    font: gridTypeComboBox.font
                }
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Grid visible")
            }

            Switch {
                id: gridVisibleSwitch
                Layout.rightMargin: 10
                checked: gridSample.gridVisible
                onCheckedChanged: gridSample.setGridVisible(checked);
            }

            Text {
                text: qsTr("Labels visible")
                Layout.leftMargin: 10
                enabled: gridVisibleSwitch.checked
                color: enabled ? palette.text : "gray"
            }

            Switch {
                id: labelVisibleSwitch
                Layout.rightMargin: 10
                checked: gridSample.labelsVisible
                enabled: gridVisibleSwitch.checked
                onCheckedChanged: gridSample.setLabelsVisible(checked);
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Grid color")
            }

            ComboBox {
                id: colorCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: gridSample.lineColors
                onCurrentTextChanged: gridSample.setLineColor(currentText);
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        colorComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: colorComboMetrics
                    font: colorCombo.font
                }
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Label color")
            }

            ComboBox {
                id: colorCombo2
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: gridSample.labelColors
                onCurrentTextChanged: gridSample.setLabelColor(currentText);
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        colorCombo2Metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorCombo2Metrics.width);
                    }
                }
                TextMetrics {
                    id: colorCombo2Metrics
                    font: colorCombo2.font
                }
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Label position")
                enabled: positionCombo.enabled
                color: enabled ? palette.text  : "gray"
            }

            ComboBox {
                id: positionCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: gridSample.labelPositions
                onCurrentTextChanged: gridSample.setLabelPosition(currentText);
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        positionComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, positionComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: positionComboMetrics
                    font: positionCombo.font
                }
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Label format")
                enabled: formatCombo.enabled
                color: enabled ? palette.text  : "gray"
            }

            ComboBox {
                id: formatCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + (indicator ? indicator.width : 10)
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: gridSample.labelFormats
                enabled: gridSample.currentGridType === "LatLong"
                onCurrentTextChanged: gridSample.setLabelFormat(currentText);
                Component.onCompleted : {
                    for (let i = 0; i < model.length; ++i) {
                        formatComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, formatComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: formatComboMetrics
                    font: formatCombo.font
                }
            }

            Label {
                Layout.leftMargin: 10
                text: qsTr("Label offset")
                enabled: offsetSlider.enabled
                color: enabled ? palette.text  : "gray"
            }

            Slider {
                id: offsetSlider
                Layout.minimumWidth: leftPadding + rightPadding
                Layout.rightMargin: 10
                Layout.fillWidth: true
                from: 0
                to: 150
                value: gridSample.currentLabelOffset
                enabled: (gridSample.currentLabelPosition !== "Geographic" && gridSample.currentLabelPosition !== "Center")
                onValueChanged: {
                    if (value !== gridSample.currentLabelOffset)
                        gridSample.setLabelOffset(value);
                }

                handle: Rectangle {
                    x: offsetSlider.leftPadding + offsetSlider.visualPosition * (offsetSlider.availableWidth - width)
                    y: offsetSlider.topPadding + offsetSlider.availableHeight / 2 - height / 2
                    implicitWidth: 26
                    implicitHeight: 26

                    color: offsetSlider.enabled ? palette.highlight : "gray"
                    border.color: "gray"
                    Label {
                        anchors.centerIn: parent
                        text: offsetSlider.value.toFixed(0)
                        font.pixelSize: 14
                        color: offsetSlider.enabled ? palette.text : "gray"
                        onWidthChanged: {
                            parent.width = Math.max(width + 10, 26)
                        }
                    }
                }
            }

            // Spacer
            Item {
                height: 8
                Layout.fillWidth: true
                Layout.columnSpan: 2
            }

            // Button to hide the styling window
            Button {
                id: hideButton
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.bottomMargin: 5

                width: 150
                height: 30
                text: qsTr("Hide window")
                font.pixelSize: 14

                onClicked: {
                    background.visible = false;
                    hideAnimation.restart();
                }
            }
        }
    }
}
