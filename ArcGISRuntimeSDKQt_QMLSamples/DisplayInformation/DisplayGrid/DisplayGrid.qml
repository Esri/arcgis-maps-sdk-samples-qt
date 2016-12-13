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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    id: rootRectangle
    clip: true

    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor
    property string utmGrid: "UTM"
    property string usngGrid: "USNG"
    property string latlonGrid: "LatLon"
    property string mgrsGrid: "MGRS"
    property string dd: "Decimal degrees"
    property string dms: "Degrees minutes seconds"
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
            ViewpointExtent {
                Envelope {
                    xMin: -11018849.838073004
                    yMin: 4906181.949903341
                    xMax: -9653433.562293354
                    yMax: 5930244.156738078
                    spatialReference: SpatialReference.createWebMercator()
                }
            }
        }
    }

    // change the grid color for each grid level
    function changeGridColor(color) {
        if (mapView.grid) {
            for (var level = 0; level < mapView.grid.levelCount; ++level) {
                var lineSym = ArcGISRuntimeEnvironment.createObject("SimpleLineSymbol", {style: Enums.SimpleLineSymbolStyleSolid,
                                                                        color: color,
                                                                        width: 1 + level} );
                mapView.grid.setLineSymbol(level, lineSym);
            }
        }
    }

    // change the grid label color for each grid level
    function changeLabelColor(color) {
        if (mapView.grid) {
            for (var level = 0; level < mapView.grid.levelCount; ++level) {
                var textSym = ArcGISRuntimeEnvironment.createObject("TextSymbol", {
                                                                        text: "text",
                                                                        size: 14,
                                                                        color: color,
                                                                        horizontalAlignment: Enums.HorizontalAlignmentLeft,
                                                                        verticalAlignment: Enums.VerticalAlignmentBottom
                                                                    });

                textSym.outlineColor = "black";
                textSym.outlineWidth = 2;

                textSym.haloColor = "white";
                textSym.haloWidth = 2 + level;

                mapView.grid.setTextSymbol(level, textSym);
            }
        }
    }

    // change the grid label format
    function changeLabelFormat(format) {
        if (mapView.grid) {
            if (mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid) {
                if (format === dd)
                    mapView.grid.labelFormat = Enums.LatitudeLongitudeGridLabelFormatDecimalDegrees;
                else if (format === dms)
                    mapView.grid.labelFormat = Enums.LatitudeLongitudeGridLabelFormatDegreesMinutesSeconds;
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
           bottomMargin: 23 * scaleFactor
       }

       width: 150 * scaleFactor
       height: 30 * scaleFactor
       color: pressed ? "#959595" : "#D6D6D6"
       radius: 8
       border {
           color: "#585858"
           width: 1 * scaleFactor
       }


       Text {
           anchors.centerIn: parent
           text: "Change grid style"
           font.pixelSize: 14 * scaleFactor
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
            width: 0.5 * scaleFactor
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
        width: 225 * scaleFactor
        height: 250 * scaleFactor
        color: "lightgray"
        radius: 4 * scaleFactor
        border {
            color: "black"
            width: 1 * scaleFactor
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
            anchors {
                fill: parent
                margins: 10 * scaleFactor
            }
            columns: 2
            columnSpacing: 10
            rows: 7
            rowSpacing: 10

            Text {
                text: "Grid type"
                Layout.maximumWidth: styleWindow.width * 0.35
            }

            ComboBox {
                id: gridTypeComboBox
                model: [latlonGrid, mgrsGrid, utmGrid, usngGrid]
                Layout.minimumWidth: styleWindow.width * 0.5

                onCurrentTextChanged: {
                    // change the grid
                    if (currentText === latlonGrid)
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("LatitudeLongitudeGrid");
                    else if (currentText === utmGrid)
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("UTMGrid");
                    else if (currentText === usngGrid)
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("USNGGrid");
                    else if (currentText === mgrsGrid)
                        mapView.grid = ArcGISRuntimeEnvironment.createObject("MGRSGrid");
                    else
                        return;

                    // apply any styling that has been set
                    changeGridColor(currentGridColor);
                    changeLabelColor(currentGridLabelColor);
                    changeLabelFormat(currentLabelFormat);
                    mapView.grid.visible = gridVisible;
                    mapView.grid.labelsVisible = labelVisible;
                }
            }

            Text {
                text: "Labels visible"
                enabled: gridVisibleSwitch.checked
                Layout.maximumWidth: styleWindow.width * 0.35
                color: enabled ? "black" : "gray"
            }

            Switch {
                id: labelVisibleSwitch
                checked: true
                enabled: gridVisibleSwitch.checked
                Layout.minimumWidth: styleWindow.width * 0.5
                Layout.leftMargin: styleWindow.width * 0.25

                onCheckedChanged: {
                    if (checked)
                        mapView.grid.labelsVisible = true;
                    else
                        mapView.grid.labelsVisible = false;
                }
            }

            Text {
                text: "Grid visible"
                Layout.maximumWidth: styleWindow.width * 0.35
            }

            Switch {
                id: gridVisibleSwitch
                checked: true
                Layout.minimumWidth: styleWindow.width * 0.5
                Layout.leftMargin: styleWindow.width * 0.25

                onCheckedChanged: {
                    if (checked)
                        mapView.grid.visible = true;
                    else
                        mapView.grid.visible = false;
                }
            }

            Text {
                text: "Grid color"
                Layout.maximumWidth: styleWindow.width * 0.35
            }

            ComboBox {
                model: ["red", "white", "blue"]
                Layout.minimumWidth: styleWindow.width * 0.5

                onCurrentTextChanged: {
                    currentGridColor = currentText;
                    changeGridColor(currentGridColor);
                }
            }

            Text {
                text: "Label color"
                Layout.maximumWidth: styleWindow.width * 0.35
            }

            ComboBox {
                model: ["red", "black", "blue"]
                Layout.minimumWidth: styleWindow.width * 0.5

                onCurrentTextChanged: {
                    currentGridLabelColor = currentText;
                    changeLabelColor(currentGridLabelColor);
                }
            }

            Text {
                text: "Label format"
                Layout.maximumWidth: styleWindow.width * 0.35
                enabled: mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid
                color: enabled ? "black"  : "gray"
            }

            ComboBox {
                model: [dd, dms]
                Layout.minimumWidth: styleWindow.width * 0.5
                enabled: mapView.grid.gridType === Enums.GridTypeLatitudeLongitudeGrid

                onCurrentTextChanged: {
                    currentLabelFormat = currentText;
                    changeLabelFormat(currentLabelFormat);
                }
            }

            // Button to hide the styling window
            Rectangle {
               id: hideButton
               property bool pressed: false
               anchors.horizontalCenter: parent.horizontalCenter
               Layout.columnSpan: 2

               width: 150 * scaleFactor
               height: 30 * scaleFactor
               color: pressed ? "#959595" : "#D6D6D6"
               radius: 8
               border {
                   color: "#585858"
                   width: 1 * scaleFactor
               }


               Text {
                   anchors.centerIn: parent
                   text: "Hide window"
                   font.pixelSize: 14 * scaleFactor
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

        // Neatline rectangle
        Rectangle {
            id: neatline
            anchors.fill: parent
            color: "transparent"
            opacity: 0.0
            border {
                width: 0.5 * scaleFactor
                color: "black"
            }
        }
    }
}
