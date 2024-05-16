// [WriteFile Name=SnapGeometryEdits, Category=EditData]
// [Legal]
// Copyright 2024 Esri.

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
import QtQuick.Layouts
import Esri.Samples

Item {
    // property var phases: ["System Value", "Service Connection", "Curb Stop Value"]
    property var pointLayers: snapGeometryEditsSampleModel.pointLayers
    property var polylineLayers: ["Lateral","Main"]
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    SnapGeometryEditsSample {
        id: snapGeometryEditsSampleModel
        mapView: view
    }

Rectangle {

    anchors {
        right: parent.right
        margins: 10
    }

    Control {
        id: control
        anchors.right: parent.right
        padding: 5
        width: 140

        background: Rectangle {
            color: "black"
            opacity: .5
        }

        contentItem: ColumnLayout {
            id: columns
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            GridLayout {
                id: geometryColumn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Text {
                    id: geometryHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: "Create"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                GeometryEditorButton {
                    id: pointButton
                    buttonName: qsTr("Point")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/point-32.png"
                    checkable: true
                    enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                    onClicked: {
                        snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PointMode);
                    }
                }

                GeometryEditorButton {
                    id: multiPointButton
                    buttonName: qsTr("Multipoint")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/multipoint-32.png"
                    checkable: true
                    enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                    onClicked: {
                        snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.MultipointMode);
                    }
                }

                GeometryEditorButton {
                    id: lineButton
                    buttonName: qsTr("Line")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/line-32.png"
                    checkable: true
                    enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                    onClicked: {
                        snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PolylineMode);
                    }
                }

                GeometryEditorButton {
                    id: polygonButton
                    buttonName: qsTr("Polygon")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/polygon-32.png"
                    checkable: true
                    enabled: !snapGeometryEditsSampleModel.geometryEditorStarted
                    onClicked: {
                        snapGeometryEditsSampleModel.startEditor(SnapGeometryEditsSample.PolygonMode);
                    }
                }
            }

            GridLayout {
                id: editingColumn
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Text {
                    id: editingHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("Edit")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                GeometryEditorButton {
                    id: undoButton
                    buttonName: qsTr("Undo")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/undo-32.png"
                    enabled: snapGeometryEditsSampleModel.geometryEditorStarted && snapGeometryEditsSampleModel.canUndo
                    onClicked: snapGeometryEditsSampleModel.editorUndo();
                }

                GeometryEditorButton {
                    id: deleteVertexButton
                    buttonName: qsTr("Delete")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/erase-32.png"
                    // Layout.columnSpan: 2
                    enabled: snapGeometryEditsSampleModel.geometryEditorStarted || snapGeometryEditsSampleModel.elementIsSelected
                    onClicked: snapGeometryEditsSampleModel.deleteSelection();
                }

                GeometryEditorButton {
                    id: saveEditsButton
                    buttonName: qsTr("Save")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/save-32.png"
                    Layout.columnSpan: 2
                    enabled: snapGeometryEditsSampleModel.geometryEditorStarted
                    onClicked: {
                        snapGeometryEditsSampleModel.stopEditor();
                    }
                }

                GeometryEditorButton {
                    id: snapSettingsButton
                    buttonName: qsTr("Snap Settings")
                    iconPath: "qrc:/Samples/EditData/SnapGeometryEdits/iconAssets/settings.png"
                    Layout.columnSpan: 2
                    enabled: true
                    // MouseArea {
                    //                 anchors.fill: parent
                    //                 onClicked: optionPanel.visible = true;
                    // }
                    onClicked: {
                        optionPanel.visible = true;
                        snapGeometryEditsSampleModel.configureSnapping();
                    }
                }
            }
        }
    }
}
    Rectangle {
        id: optionPanel
        anchors {
            right: parent.right
            top: parent.top
            // bottom: sceneView.attributionTop
            bottom: parent.bottom
        }
        width: 360
        visible: false
        color: "white"
        opacity: 1

        Flickable {
            anchors {
                fill: parent
                margins: 10
            }
            contentWidth: parent.width
            contentHeight: parent.height
            flickableDirection: Flickable.VerticalFlick
            ColumnLayout
            {
                id: layout
                spacing: 40
            Column {
                id: optionColumn
                spacing: 10
                width: optionPanel.width

                Item {
                    width: parent.width
                    height: 25


                    Text {
                        text: "Snapping"
                        // anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 15
                        color: "#8434C1"
                        font.bold: true

                    }

                    Rectangle {
                        anchors {
                            right: parent.right
                            margins: 10
                        }
                        width: 45
                        height: 20

                        Text {
                            text: "Done"
                            font.pixelSize: 15
                            color: "#8434C1"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: optionPanel.visible = false;
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 25

                    Rectangle {
                        // anchors.centerIn: parent
                        // width: childrenRect.width
                        // height: childrenRect.height
                        // anchors.margins: 40


                        anchors {
                            // right: parent.right
                            margins: 10
                        }

                        width: 340
                        height: 45
                        color: "#E9DFEA"
                        radius: 5
                        border {
                            color: "#4D4D4D"
                            width: 1
                        }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        // width: parent.width * 0.75
                        text: qsTr("Enabled")
                        font.pixelSize: 15
                        anchors {
                            left: parent.left
                            margins: 10
                            verticalCenter: parent.verticalCenter
                        }
                    }

                    Switch {
                        anchors {
                            right: parent.right
                            margins: 10
                            verticalCenter: parent.verticalCenter
                        }
                        onCheckedChanged: snapGeometryEditsSampleModel.snappingEnabledStatus(checked)
                    }
                    }
                }
                }
                Column {
                            id: optionColumn2
                            spacing: 10
                            width: optionPanel.width
                Item {
                    width: parent.width
                    height: 25

                    Text {
                        text: "Point Layers"
                        // anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 15
                        color: "#8434C1"
                        font.bold: true

                    }


                    Rectangle {
                        anchors {
                            right: parent.right
                            margins: 10
                        }
                        width: 140
                        height: 20

                        Text {
                            text: "Enable All Sources"
                            font.pixelSize: 15
                            color: "#8434C1"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                // checkAllPoint = true
                                snapGeometryEditsSampleModel.onPointLayersEnabled()
                            }
                        }
                    }
                }

                Item {
                    id: item

                    width: parent.width
                    height: 25

                    Rectangle {
                        anchors.margins: 20
                        width: 340
                        height: grid.height
                        color: "#E9DFEA"
                        radius: 5
                        border {
                            color: "#4D4D4D"
                            width: 1
                        }

                        ColumnLayout {
                            id: grid
                            Repeater {
                                id: repeater
                                model: snapGeometryEditsSampleModel.pointLayers // Adjust the number of items as needed
                                delegate: RowLayout {
                                    Layout.margins: 10

                                    Text {
                                        id: textItem
                                        text: pointLayers[index]
                                        Layout.alignment: Qt.AlignLeft
                                        Layout.fillWidth: true // Allow text to take all available space
                                        Layout.rightMargin: 110
                                        font.pixelSize: 15
                                    }

                                    Switch {
                                        id: switchItem
                                        Layout.alignment: Qt.AlignRight
                                        // property bool localChecked: snapGeometryEditsSampleModel.snapSourceCheckedState[index]

                                        onCheckedChanged: {
                                            // if (localChecked !== checked) {
                                            //     snapGeometryEditsSampleModel.sourceEnabledStatus(checked, index)
                                            //     localChecked = checked
                                            // }
                                            snapGeometryEditsSampleModel.pointSourceEnabledStatus(checked, index)
                                        }
                                        // Bind the checked property to the localChecked property
                                        checked: snapGeometryEditsSampleModel.pointSourceCheckedState[index]
                                    }
                                }
                            }
                        }


                    }


                }

                }

                Item {
                        // width: mainColumn.width
                        height: optionColumn2.height+10 // Adjust as needed
                    }

                Column {
                            id: optionColumn3
                            spacing: 10
                            width: optionPanel.width

                Item {
                    width: parent.width
                    height: 25

                    Text {
                        text: "Polyline Layers"
                        // anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 15
                        color: "#8434C1"
                        font.bold: true

                    }


                    Rectangle {
                        anchors {
                            right: parent.right
                            margins: 10
                        }
                        width: 140
                        height: 20

                        Text {
                            text: "Enable All Sources"
                            font.pixelSize: 15
                            color: "#8434C1"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                // checkAllPoint = true
                                console.log("polyline")
                                snapGeometryEditsSampleModel.onPolylineLayersEnabled()
                            }
                        }
                    }
                }

                Item {

                    width: parent.width
                    height: 25

                    Rectangle {
                        anchors.margins: 20
                        width: 340
                        height: grid2.height
                        color: "#E9DFEA"
                        radius: 5
                        border {
                            color: "#4D4D4D"
                            width: 1
                        }

                        ColumnLayout {
                            id: grid2
                            // Layout.bottomMargin: 10
                            // Layout.topMargin: 10
                            Repeater {
                                id: repeater2
                                model: snapGeometryEditsSampleModel.polylineLayers // Adjust the number of items as needed
                                delegate: RowLayout {
                                    // Layout.bottomMargin: 10
                                    // Layout.topMargin: 10
                                    Layout.margins: 10
                                    // spacing: 10 // Adjust the default spacing between items

                                    Text {
                                        id: textItem2
                                        text: polylineLayers[index]
                                        Layout.alignment: Qt.AlignLeft
                                        Layout.fillWidth: true // Allow text to take all available space
                                        Layout.rightMargin: 200
                                        // Layout.leftMargin: 10
                                        font.pixelSize: 15
                                    }

                                    Switch {
                                        id: switchItem2
                                        Layout.alignment: Qt.AlignRight
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





