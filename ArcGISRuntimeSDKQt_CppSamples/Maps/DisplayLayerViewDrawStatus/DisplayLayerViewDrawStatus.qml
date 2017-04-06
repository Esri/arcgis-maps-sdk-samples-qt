// [WriteFile Name=DisplayLayerViewDrawStatus, Category=Maps]
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
import QtQuick.Controls.Styles 1.4
import Esri.ArcGISExtras 1.1
import Esri.Samples 1.0

DisplayLayerViewDrawStatusSample {
    id: displayLayerView
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent
        objectName: "mapView"
    }

    // table to display layer names and statuses
    TableView {
        id: tableView
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 25 * scaleFactor
        }
        height: 150 * scaleFactor
        width: 230 * scaleFactor
        model: layerViewModel
        headerVisible: false
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        opacity: 0.95

        // set number of layers states to be displayed at once
        rowDelegate: Row {
            height: tableView.height / 3
        }

        // create rectangle to frame the TableView
        style: TableViewStyle {
            backgroundColor: "transparent"
            frame: Rectangle {
                border.color: "black"
                radius: 10

                // make sure mouse actions on table do not affect map behind it
                MouseArea {
                    anchors.fill: parent
                    onClicked: mouse.accepted = true
                    onWheel: wheel.accepted = true
                }
            }
        }

        // create List Model to store Layer View States and names
        ListModel {
            id: layerViewModel
        }

        TableViewColumn {
            role: "name"
            width: tableView.width * 0.75 - tableView.anchors.margins
            delegate: Component {
                Text {
                    text: styleData.value
                    leftPadding: tableView.anchors.margins
                    renderType: Text.NativeRendering
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font {
                        weight: Font.Black
                        pixelSize: tableView.height * 0.10
                    }
                }
            }
        }

        TableViewColumn {
            role: "status"
            width: tableView.width * 0.25
            delegate: Component {
                Text {
                    text: styleData.value
                    renderType: Text.NativeRendering
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: tableView.height * 0.10
                    color: "steelblue"
                }
            }
        }
    }

    // initialize ListModel to display layer names and ViewStates
    onMapReady: {
        for (var i = 0; i < displayLayerView.layerNames.length; i++)
            layerViewModel.append({"name": displayLayerView.layerNames[i], "status": displayLayerView.layerViewStates[i]});
    }

    // adjust ListModel when layer view state changes in C++
    onStatusChanged: {
        for (var i = 0; i < displayLayerView.layerNames.length; i++) {
            layerViewModel.setProperty(i, "name", displayLayerView.layerNames[i]);
            layerViewModel.setProperty(i, "status", displayLayerView.layerViewStates[i]);
        }
    }
}
