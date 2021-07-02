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
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    SketchOnMapSample {
        id: model
        mapView: view
    }

    Column {
        id: buttonColumn
        anchors {
            top: parent.top
            right: parent.right
        }

        Column {
            id: geometryColumn
            width: 32

            Row {
                Button {
                    id: pButton
                    Rectangle {
                        Image {
                            source: "./iconAssets/point-32.svg"
                            width: geometryColumn.width
                        }
                        anchors.centerIn: parent
                    }
                    onClicked: {
                        model.setSketchCreationMode(0);
                    }
                }
            }
            Row {
                Button {
                    Image {
                        source: "./iconAssets/point-32.svg"
                        anchors.centerIn: geometryColumn.Center
                        width: geometryColumn.width
                    }
                    onClicked: model.setSketchCreationMode(1);
                }
            }
            Row {
                Button {
                    Image {
                        source: "./iconAssets/line-32.svg"
                        anchors.centerIn: geometryColumn.Center
                        width: geometryColumn.width
                    }
                    onClicked: model.setSketchCreationMode(2);
                }
            }
            Row {
                Button {
                    Image {
                        source: "./iconAssets/polygon-32.svg"
                        anchors.centerIn: geometryColumn.Center
                        width: geometryColumn.width
                    }
                    onClicked: model.setSketchCreationMode(3);
                }
            }
        }

        Row {
            Button {
                width: pButton.width / 2
                Image {
                    source: "./iconAssets/undo-32.svg"
                    anchors.centerIn: buttonColumn.Center
                    width: buttonColumn.width / 2
                }
                onClicked: model.undo();
            }
            Button {
                width: pButton.width / 2
                Image {
                    source: "./iconAssets/redo-32.svg"
                    anchors.centerIn: buttonColumn.Center
                    width: buttonColumn.width / 2
                }
                onClicked: model.redo();
            }
        }

        Row {
            Button {
                width: pButton.width
                Image {
                    source: "./iconAssets/check-circle-32.svg"
                    width: buttonColumn.width
                    anchors.centerIn: buttonColumn.Center
                }
                onClicked: model.stopSketching(true);
            }
        }
        Row {
            Button {
                width: pButton.width
                Image {
                    source: "./iconAssets/circle-disallowed-32.svg"
                    width: buttonColumn.width
                    anchors.centerIn: buttonColumn.Center
                }
                onClicked: model.stopSketching(false);
            }
        }
        Row {
            Button {
                width: pButton.width
                Image {
                    source: "./iconAssets/trash-32.svg"
                    width: buttonColumn.width
                    anchors.centerIn: buttonColumn.Center
                }
                onClicked: model.clearGraphics();
            }
        }
    }
}
