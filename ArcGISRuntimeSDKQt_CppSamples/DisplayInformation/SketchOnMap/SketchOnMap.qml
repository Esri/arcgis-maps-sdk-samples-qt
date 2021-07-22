// [WriteFile Name=SketchOnMap, Category=DisplayInformation]
// [Legal]
// Copyright 2020 Esri.

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
                    id: geometryText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("Geometry")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }

                RoundButton {
                    id: ptButton
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    radius: 5
                    highlighted: drawStatus === "point"

                    Image {
                        id: ptImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -ptText.height/2
                        }
                        source: "iconAssets/point-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: ptText
                        text: qsTr("Point")
                        font.pixelSize: 8
                        anchors {
                            top: ptImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                    }

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.PointSketchMode);
                        drawStatus = "point";
                    }
                }

                RoundButton {
                    id: mPtButton
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    radius: 5
                    highlighted: drawStatus === "multiPoint"

                    Image {
                        id: mptImage1
                        anchors {
                            right: parent.horizontalCenter
                            rightMargin: -mptImage2.width/5
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -mptText.height/2 - 2
                        }
                        source: "iconAssets/point-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Image {
                        id: mptImage2
                        anchors {
                            left: parent.horizontalCenter
                            leftMargin: -mptImage1.width/5
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -mptText.height/2
                        }
                        source: "iconAssets/point-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: mptText
                        anchors {
                            top: mptImage2.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Multipoint")
                        font.pixelSize: 8
                    }
                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.MultipointSketchMode);
                        drawStatus = "multiPoint";
                    }
                }

                RoundButton {
                    id: lineButton
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    radius: 5
                    highlighted: drawStatus === "line"

                    Image {
                        id: lineImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -lineText.height/2
                        }
                        source: "iconAssets/line-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: lineText
                        anchors {
                            top: lineImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Line")
                        font.pixelSize: 8
                    }

                    onClicked: {
                        model.setSketchCreationMode(SketchOnMapSample.PolylineSketchMode);
                        drawStatus = "line";
                    }
                }

                RoundButton {
                    id: polygonButton
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    radius: 5
                    highlighted: drawStatus === "polygon"

                    Image {
                        id: polygonImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -polygonText.height/2
                        }
                        source: "iconAssets/polygon-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: polygonText
                        anchors {
                            top: polygonImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Polygon")
                        font.pixelSize: 8
                    }

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
                    id: editingText
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.columnSpan: 2
                    text: qsTr("Editing")
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }


                SketchEditorButton {
                    id: undoButton
                    buttonName: "undo"
                    iconPath: "iconAssets/undo-32.png"
                    onClicked: model.undo();
//                    Layout.fillWidth: true
//                    radius: 5

//                    Image {
//                        id: undoImage
//                        anchors {
//                            horizontalCenter: parent.horizontalCenter
//                            verticalCenter: parent.verticalCenter
//                            verticalCenterOffset: -undoText.height/2
//                        }
//                        source: "iconAssets/undo-32.png"
//                        width: 20
//                        fillMode: Image.PreserveAspectFit
//                    }

//                    Text {
//                        id: undoText
//                        anchors {
//                            top: undoImage.bottom
//                            horizontalCenter: parent.horizontalCenter
//                        }
//                        text: qsTr("Undo")
//                        font.pixelSize: 8
//                    }

//                    onClicked: model.undo();
                }

                RoundButton {
                    id: redoButton
                    Layout.fillWidth: true
                    radius: 5
                    Image {
                        id: redoImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -redoText.height/2
                        }
                        source: "iconAssets/redo-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: redoText
                        anchors {
                            top: redoImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Redo")
                        font.pixelSize: 8
                    }

                    onClicked: model.redo();
                }


                RoundButton {
                    id: saveEditsButton
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    radius: 5

                    Image {
                        id: saveEditsImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -saveEditsText.height/2
                        }
                        source: "iconAssets/check-circle-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: saveEditsText
                        anchors {
                            top: saveEditsImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Save edits")
                        font.pixelSize: 8
                    }

                    onClicked: {
                        model.stopSketching(true);
                        drawStatus = "";
                    }
                }

                RoundButton {
                    id: discardEditsButton
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    radius: 5

                    Image {
                        id: discardEditsImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -discardEditsText.height/2
                        }
                        source: "iconAssets/circle-disallowed-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: discardEditsText
                        anchors {
                            top: discardEditsImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Discard edits")
                        font.pixelSize: 8
                    }

                    onClicked: {
                        model.stopSketching(false);
                        drawStatus = "";
                    }
                }

                RoundButton {
                    id: clearGraphicsButton
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    radius: 5

                    Image {
                        id: clearGraphicsImage
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: -clearGraphicsText.height/2
                        }
                        source: "iconAssets/trash-32.png"
                        width: 20
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: clearGraphicsText
                        anchors {
                            top: clearGraphicsImage.bottom
                            horizontalCenter: parent.horizontalCenter
                        }
                        text: qsTr("Clear graphics")
                        font.pixelSize: 8
                    }

                    onClicked: model.clearGraphics();
                }
            }
        }
    }
}
