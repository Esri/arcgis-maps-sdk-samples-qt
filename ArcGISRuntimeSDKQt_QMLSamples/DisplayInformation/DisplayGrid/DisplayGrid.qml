// [WriteFile Name=DisplayGrid, Category=DisplayInformation]
// [Legal]
// Copyright 2016 Esri.

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

import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import Esri.ArcGISRuntime 100.5
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    
    property string utmGrid: "UTM"
    property string usngGrid: "USNG"
    property string latlonGrid: "LatLon"
    property string mgrsGrid: "MGRS"
    property string dd: "Decimal degrees"
    property string dms: "Degrees minutes seconds"
    property string geographic: "Geographic"
    property string bottomLeft: "Bottom left"
    property string bottomRight: "Bottom right"
    property string topLeft: "Top left"
    property string topRight: "Top right"
    property string center: "Center"
    property string allSides: "All sides"
    property string currentLabelPosition: geographic
    property real centerWindowY: (rootRectangle.height / 2) - (styleWindow.height / 2)
    property color currentGridColor: "red"
    property color currentGridLabelColor: "black"
    property string currentLabelFormat
    property alias labelVisible: labelVisibleSwitch.checked
    property bool gridVisible: gridVisibleSwitch.checked

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            BasemapImagery {}

            // Set an initial viewpoint
            ViewpointCenter {
                targetScale: 6450785

                Point {
                    x: -10336141.70018318
                    y: 5418213.05332071
                    spatialReference: SpatialReference { wkid: 3857 }
                }
            }
        }
    }

    // change the grid color for each grid level
    function changeGridColor(color) {
        if (mapView.grid) {
            // find the number of resolution levels in the grid
            var gridLevels = mapView.grid.levelCount;

            //! [DisplayGrid Set_Grid_Lines]
            for (var level = 0; level < gridLevels; ++level) {
                var lineSym = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol", {style: Enums.SimpleLineSymbolStyleSolid,
                                                                        color: color,
                                                                        width: 1 + level} );
                mapView.grid.setLineSymbol(level, lineSym);
            }
            //! [DisplayGrid Set_Grid_Lines]
        }
    }

    // change the grid label color for each grid level
    function changeLabelColor(color) {
        if (mapView.grid) {
            //! [DisplayGrid Grid_Levels]
            // find the number of resolution levels in the grid
            var gridLevels = mapView.grid.levelCount;
            //! [DisplayGrid Grid_Levels]

            //! [DisplayGrid Set_Grid_Labels]
            for (var level = 0; level < gridLevels; ++level) {
                var textSym = ArcGISRuntimeEnvironment.createObject("TextSymbol", {
                                                                        text: "text",
                                                                        size: 14,
                                                                        color: color,
                                                                        horizontalAlignment: Enums.HorizontalAlignmentLeft,
                                                                        verticalAlignment: Enums.VerticalAlignmentBottom
                                                                    });
                textSym.haloColor = "white";
                textSym.haloWidth = 2 + level;

                mapView.grid.setTextSymbol(level, textSym);
            }
            //! [DisplayGrid Set_Grid_Labels]
        }
    }

    // change the grid label format
    function changeLabelFormat(format) {
        if (mapView.grid) {
            if (mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid) {
                if (format === dd) {
                    // format the labels to display in decimal degrees
                    mapView.grid.labelFormat = Enums.LatitudeLongitudeGridLabelFormatDecimalDegrees;
                } else if (format === dms) {
                    //! [DisplayGrid Set_Label_Format]
                    // format the labels to display in degrees, minutes and seconds
                    mapView.grid.labelFormat = Enums.LatitudeLongitudeGridLabelFormatDegreesMinutesSeconds;
                    //! [DisplayGrid Set_Label_Format]
                }
            }
        }
    }

    // change the grid label placement
    function changeLabelPosition(position) {
        if (mapView.grid) {
            switch(position) {
            case geographic:
                //! [DisplayGrid Set_Label_Placement_Geographic]
                // update the label positioning to geographic
                mapView.grid.labelPosition = Enums.GridLabelPositionGeographic;
                //! [DisplayGrid Set_Label_Placement_Geographic]
                break;
            case bottomLeft:
                // update the label positioning to bottom left
                mapView.grid.labelPosition = Enums.GridLabelPositionBottomLeft;
                break;
            case bottomRight:
                // update the label positioning to bottom right
                mapView.grid.labelPosition = Enums.GridLabelPositionBottomRight;
                break;
            case topLeft:
                // update the label positioning to top left
                mapView.grid.labelPosition = Enums.GridLabelPositionTopLeft;
                break;
            case topRight:
                //! [DisplayGrid Set_Label_Placement_Top_Right]
                // update the label positioning to top right
                mapView.grid.labelPosition = Enums.GridLabelPositionTopRight;
                //! [DisplayGrid Set_Label_Placement_Top_Right]
                break;
            case center:
                // update the label positioning to center
                mapView.grid.labelPosition = Enums.GridLabelPositionCenter;
                break;
            case allSides:
                // update the label positioning to all sides
                mapView.grid.labelPosition = Enums.GridLabelPositionAllSides;
                break;
            default:
                mapView.grid.labelPosition = Enums.GridLabelPositionGeographic;
            }
        }
    }

    // Button to view the styling window
    Rectangle {
        id: styleButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 23
        }

        width: 150
        height: 30
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 8
        border {
            color: "#585858"
            width: 1
        }


        Text {
            anchors.centerIn: parent
            text: "Change grid style"
            font.pixelSize: 14
            color: "#474747"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: styleButton.pressed = true
            onReleased: styleButton.pressed = false
            onClicked: {
                background.visible = true;
                showAnimation.restart()
            }
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
            onClicked: mouse.accepted = true;
        }
    }

    Rectangle {
        id: styleWindow
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height // initially display below the page
        height: childrenRect.height
        width: childrenRect.width

        color: "lightgray"
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
            NumberAnimation { target: styleWindow; property: "y"; to: rootRectangle.height; duration: 200 }
        }

        GridLayout {
            id: grid
            columns: 2

            Text {
                Layout.leftMargin: 10
                text: "Grid type"
            }

            ComboBox {
                id: gridTypeComboBox
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [latlonGrid, mgrsGrid, utmGrid, usngGrid]

                onCurrentTextChanged: {
                    // change the grid
                    if (currentText === latlonGrid) {
                        //! [DisplayGrid Set_LatLon_Grid]
                        // create a grid for showing Latitude and Longitude (Meridians and Parallels)
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("LatitudeLongitudeGrid");
                        //! [DisplayGrid Set_LatLon_Grid]
                    } else if (currentText === utmGrid) {
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("UTMGrid");
                    } else if (currentText === usngGrid) {
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("USNGGrid");
                    } else if (currentText === mgrsGrid) {
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("MGRSGrid");
                    } else
                        return;

                    // apply any styling that has been set
                    changeGridColor(currentGridColor);
                    changeLabelColor(currentGridLabelColor);
                    changeLabelFormat(currentLabelFormat);
                    changeLabelPosition(currentLabelPosition);
                    mapView.grid.visible = gridVisible;
                    mapView.grid.labelsVisible = labelVisible;
                }

                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        metricsGridTypeComboBox.text = model[i];
                        modelWidth = Math.max(modelWidth, metricsGridTypeComboBox.width);
                    }
                }
                TextMetrics {
                    id: metricsGridTypeComboBox
                    font: gridTypeComboBox.font
                }
            }

            Text {
                text: "Labels visible"
                Layout.leftMargin: 10
                enabled: gridVisibleSwitch.checked
                color: enabled ? "black" : "gray"
            }

            Switch {
                id: labelVisibleSwitch
                Layout.rightMargin: 10
                checked: true
                enabled: gridVisibleSwitch.checked

                onCheckedChanged: {
                    if (checked) {
                        mapView.grid.labelsVisible = true;
                    } else {
                        //! [DisplayGrid Label_Visible]
                        mapView.grid.labelsVisible = false;
                        //! [DisplayGrid Label_Visible]
                    }
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Grid visible"
            }

            Switch {
                id: gridVisibleSwitch
                Layout.rightMargin: 10
                checked: true
                onCheckedChanged: {
                    if (checked) {
                        mapView.grid.visible = true;
                    } else {
                        //! [DisplayGrid Grid_Visible]
                        mapView.grid.visible = false;
                        //! [DisplayGrid Grid_Visible]
                    }
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Grid color"
            }

            ComboBox {
                id: colorCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width

                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: ["red", "white", "blue"]

                onCurrentTextChanged: {
                    currentGridColor = currentText;
                    changeGridColor(currentGridColor);
                }

                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        colorComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: colorComboMetrics
                    font: colorCombo.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label color"
            }

            ComboBox {
                id: colorCombo2
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: ["red", "black", "blue"]

                onCurrentTextChanged: {
                    currentGridLabelColor = currentText;
                    changeLabelColor(currentGridLabelColor);
                }
                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        colorCombo2Metrics.text = model[i];
                        modelWidth = Math.max(modelWidth, colorCombo2Metrics.width);
                    }
                }
                TextMetrics {
                    id: colorCombo2Metrics
                    font: colorCombo2.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label position"
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                id: positionCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [geographic, bottomLeft, bottomRight, topLeft, topRight, center, allSides]

                onCurrentTextChanged: {
                    currentLabelPosition = currentText;
                    changeLabelPosition(currentLabelPosition);
                }

                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        positionComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, positionComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: positionComboMetrics
                    font: positionCombo.font
                }
            }

            Text {
                Layout.leftMargin: 10
                text: "Label format"
                enabled: mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                id: formatCombo
                property int modelWidth: 0
                Layout.minimumWidth: modelWidth + leftPadding + rightPadding + indicator.width
                Layout.rightMargin: 10
                Layout.fillWidth: true
                model: [dd, dms]
                enabled: mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid

                onCurrentTextChanged: {
                    currentLabelFormat = currentText;
                    changeLabelFormat(currentLabelFormat);
                }
                Component.onCompleted : {
                    for (var i = 0; i < model.length; ++i) {
                        formatComboMetrics.text = model[i];
                        modelWidth = Math.max(modelWidth, formatComboMetrics.width);
                    }
                }
                TextMetrics {
                    id: formatComboMetrics
                    font: formatCombo.font
                }
            }

            // Button to hide the styling window
            Rectangle {
                id: hideButton
                property bool pressed: false
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.bottomMargin: 5

                width: 150
                height: 30
                color: pressed ? "#959595" : "#D6D6D6"
                radius: 8
                border {
                    color: "#585858"
                    width: 1
                }

                Text {
                    anchors.centerIn: parent
                    text: "Hide window"
                    font.pixelSize: 14
                    color: "#474747"
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: hideButton.pressed = true
                    onReleased: hideButton.pressed = false
                    onClicked: {
                        background.visible = false;
                        hideAnimation.restart()
                    }
                }
            }
        }
    }
}
