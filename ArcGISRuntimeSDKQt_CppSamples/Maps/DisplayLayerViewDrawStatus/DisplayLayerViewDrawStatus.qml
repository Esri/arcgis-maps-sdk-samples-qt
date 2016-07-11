
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

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
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
    }

    // table to display layer names and statuses
    TableView {
        id: tableView
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 10 * scaleFactor
        }
        height: 150 * scaleFactor
        width: 230 * scaleFactor
        model: layerViewModel
        headerVisible: false
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        section.labelPositioning: 10
        opacity: parent.opacity

        // set number of layers states to be displayed at once
        rowDelegate: Row {
            height: tableView.height / 3
        }

        style: TableViewStyle {
            backgroundColor: "transparent"
            frame: Rectangle {
                border.color: "black"
                opacity: 0.77
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
            width: tableView.width * 0.75 - 10 * scaleFactor
            delegate: Component {
                Text {
                    text: styleData.value
                    renderType: Text.NativeRendering
                    font.weight: Font.Black
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: tableView.height * 0.10
                    leftPadding: 10 * scaleFactor
                    elide: Text.ElideRight
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

    // Neatline Rectangle
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }

    // initialize ListModel to display layer names and ViewStates
    Component.onCompleted: {
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
