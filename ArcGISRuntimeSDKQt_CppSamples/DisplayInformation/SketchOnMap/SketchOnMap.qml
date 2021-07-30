// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2021 Esri.

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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.15
import Esri.Samples 1.0

Item {

    property var drawStatus: ""

    SketchOnMapSample {
        id: model
        mapView: view
    }

    MapView {
        id: view
        anchors.fill: parent
    }

    // Display an option to delete the selected vertex if the user right-clicks or taps and holds when the sketch editor is started
    MouseArea {
        anchors.fill: parent
        acceptedButtons: {
            Qt.RightButton
            Qt.TapAndHoldGesture
        }
        enabled: drawStatus !== ""
        onClicked: {
            contextMenu.popup()
        }

        Menu {
            id: contextMenu
            width: 75
            Action {
                text: "Delete"
                onTriggered: model.deleteVertex();
            }
        }
    }

    Control {
        id: control
        anchors.right: parent.right
        padding: 5
        width: 110

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
            spacing: 20

            GridLayout {
                id: geometryColumn
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                columns: 2

                Text {
                    id: geometryHeader
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("Geometry")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                SketchEditorButton {
                    id: ptButton
                    buttonName: "Point"
                    iconPath: "iconAssets/point-32.png"

                    highlighted: drawStatus === "point"
                    enabled: !model.sketchEditorStarted

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.PointSketchMode);
                        drawStatus = "point";
                    }
                }

                SketchEditorButton {
                    id: mPtButton
                    buttonName: "Multipoint"
                    iconPath: "iconAssets/point-32.png"
                    images: 2

                    highlighted: drawStatus === "multiPoint"
                    enabled: !model.sketchEditorStarted

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.MultipointSketchMode);
                        drawStatus = "multiPoint";
                    }
                }

                SketchEditorButton {
                    id: lineButton
                    buttonName: "Line"
                    iconPath: "iconAssets/line-32.png"

                    highlighted: drawStatus === "line"
                    enabled: !model.sketchEditorStarted

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.PolylineSketchMode);
                        drawStatus = "line";
                    }
                }

                SketchEditorButton {
                    id: polygonButton
                    buttonName: "Polygon"
                    iconPath: "iconAssets/polygon-32.png"

                    highlighted: drawStatus === "polygon"
                    enabled: !model.sketchEditorStarted

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.PolygonSketchMode);
                        drawStatus = "polygon";
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
                    text: qsTr("Editing")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }


                SketchEditorButton {
                    id: undoButton
                    buttonName: "Undo"
                    iconPath: "iconAssets/undo-32.png"

                    enabled: model.sketchEditorStarted

                    onClicked: model.undo();
                }

                SketchEditorButton {
                    id: redoButton
                    buttonName: "Redo"
                    iconPath: "iconAssets/redo-32.png"

                    enabled: model.sketchEditorStarted

                    onClicked: model.redo();
                }

                SketchEditorButton {
                    id: saveEditsButton
                    columnSpan: 2
                    buttonName: "Save edits"
                    iconPath: "iconAssets/check-circle-32.png"

                    enabled: model.sketchEditorStarted

                    onClicked: {
                        model.stopSketching(true);
                        drawStatus = "";
                        if (!model.sketchEditorStarted && !clearGraphicsButton.enabled)
                            clearGraphicsButton.enabled = true;
                    }
                }

                SketchEditorButton {
                    id: discardEditsButton
                    columnSpan: 2
                    buttonName: "Discard edits"
                    iconPath: "iconAssets/circle-disallowed-32.png"

                    enabled: model.sketchEditorStarted

                    onClicked: {
                        model.stopSketching(false);
                        drawStatus = "";
                    }
                }

                SketchEditorButton {
                    id: clearGraphicsButton
                    columnSpan: 2
                    buttonName: "Clear graphics"
                    iconPath: "iconAssets/trash-32.png"

                    enabled: false

                    onClicked: {
                        model.clearGraphics();
                        enabled = false;
                    }
                }
            }
        }
    }
}
