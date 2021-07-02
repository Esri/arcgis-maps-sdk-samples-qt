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
        // width: 200
        Row {
            Button {
                id: pButton
                text: "Point"
                onClicked: {
                    model.setSketchCreationMode(0);
                }
            }
        }
        Row {
            Button {
                text: "MultiPoint"
                onClicked: model.setSketchCreationMode(1);
            }
        }
        Row {
            Button {
                text: "Polyline"
                onClicked: model.setSketchCreationMode(2);
            }
        }
        Row {
            Button {
                text: "Polygon"
                onClicked: model.setSketchCreationMode(3);
            }
        }

        Row {
            Button {
                width: pButton.width / 2
                text: "Undo"
                onClicked: model.undo();
            }
            Button {
                width: pButton.width / 2
                text: "Redo"
                onClicked: model.redo();
            }
        }

        Row {
            Button {
                width: pButton.width
                text: "Stop"
                onClicked: model.stopSketching();
            }
        }
        Row {
            Button {
                width: pButton.width
                text: "Clear graphics"
                onClicked: model.clearGraphics();
            }
        }
    }
}
