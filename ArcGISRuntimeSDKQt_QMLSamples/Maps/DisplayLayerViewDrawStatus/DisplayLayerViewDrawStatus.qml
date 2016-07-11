
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
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

Rectangle {
    width: 800
    height: 600

    property real scaleFactor: System.displayScaleFactor

    // add a mapView component
    MapView {
        anchors.fill: parent

        // add a map to the mapView
        Map {
            id: map

            // create tiled layer using url
            ArcGISTiledLayer {
                url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/WorldTimeZones/MapServer"
            }

            // create a map image layer using a url
            ArcGISMapImageLayer {
                url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Census/MapServer"
                minScale: 40000000
                maxScale: 2000000
            }

            //create a feature layer using a url
            FeatureLayer {

                ServiceFeatureTable {
                    url: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Recreation/FeatureServer/0"
                }
            }

            // create initial viewpoint
            ViewpointCenter {
                scale: 5e7

                Point {
                    x: -11e6
                    y: 45e5
                    spatialReference: SpatialReference {
                        wkid: 102100
                    }
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded)
                    for (var i = 0; i < map.operationalLayers.count; i++)
                        layerViewModel.append({"name": map.operationalLayers.get(i).name, "status": "Unknown"});
            }
        }

        onLayerViewStateChanged: {
            // find index of changed layer
            var index = getindex(layer);
            // get Current Status
            var status = viewStatusString(layerViewState);
            // change name if layer loaded
            layerViewModel.setProperty(index, "name", layer.name);
            // update Status in ListModel
            layerViewModel.setProperty(index, "status", status);
        }

        function viewStatusString(layerViewState) {
            switch(layerViewState.status) {
            case Enums.LayerViewStatusActive:
                return "Active";
            case Enums.LayerViewStatusNotVisible:
                return "Not Visible";
            case Enums.LayerViewStatusOutOfScale:
                return "Out of Scale";
            case Enums.LayerViewStatusLoading:
                return "Loading";
            case Enums.LayerViewStatusError:
                return "Error";
            default:
                return "Unknown";
            }
        }

        function getindex(layer) {
            for (var i = 0; i < layerViewModel.count; i++)
                if (layer === map.operationalLayers.get(i))
                    return i;
        }
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

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
