// [WriteFile Name=ManageOperationalLayers, Category=Layers]
// [Legal]
// Copyright 2019 Esri.

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
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import Esri.ArcGISRuntime 100.9
import Esri.Samples 1.0

Rectangle {
    id: rootRectangle
    clip: true
    width: 800
    height: 600

    property var deletedLayerListModel: []
    property alias layerListModel: map.operationalLayers

    MapView {
        id: mapView
        anchors.fill: parent

        Map {
            id: map
            BasemapTopographic {}

            onLoadStatusChanged: {
                if (loadStatus !== Enums.LoadStatusLoaded)
                    return;

                // add layers to the map
                operationalLayers.append(elevationLayer);
                operationalLayers.append(censusLayer);
                operationalLayers.append(damageLayer);

                drawOrderModel.setLayerListModel(operationalLayers);
            }
        }
    }

    ArcGISMapImageLayer {
        id: elevationLayer
        url: "https://sampleserver5.arcgisonline.com/arcgis/rest/services/Elevation/WorldElevations/MapServer"
    }

    ArcGISMapImageLayer {
        id: censusLayer
        url: "https://sampleserver5.arcgisonline.com/arcgis/rest/services/Census/MapServer"
    }

    ArcGISMapImageLayer {
        id: damageLayer
        url: "https://sampleserver5.arcgisonline.com/arcgis/rest/services/DamageAssessment/MapServer"
    }

    // Declare a custom DrawOrderListModel. This is a QSortFilterProxyModel used to
    // "reverse" the order appearance so the top level layer displays at the top
    // of the list view instead of the bottom, which is the default behavior. This
    // will closer match the widely accepted standard table of contents UX where
    // the top level layer displays at the top of the view.
    DrawOrderListModel {
        id: drawOrderModel
    }

    Rectangle {
        anchors {
            fill: layerListColumn
            margins: -3
        }
        color: "#F5F5F5"
    }

    Column {
        id: layerListColumn
        anchors {
            top: parent.top
            left: parent.left
            margins: 8
        }
        spacing: 2

        Label {
            text: "Layers in map"
            visible: layersList.count > 0
            font {
                pixelSize: 14
                bold: true
            }
        }

        // Declare the ListView, which will display the list of files
        ListView {
            id: layersList
            height: contentHeight
            width: 200
            interactive: true
            clip: true
            visible: count > 0
            spacing: 5
            model: drawOrderModel
            delegate: Component {
                RowLayout {
                    width: parent.width
                    height: 40
                    spacing: 5

                    Label {
                        text: name
                        Layout.leftMargin: 5
                    }

                    Image {
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 25
                        Layout.preferredWidth: 25
                        source: "qrc:/Samples/Layers/ManageOperationalLayers/menu.png"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                layersList.currentIndex = index;
                                menu.open();
                            }
                        }

                        Menu {
                            id: menu
                            width: 125

                            Column {
                                width: parent.width
                                spacing: 10

                                Label {
                                    id: moveUpLabel
                                    text: "  Move down  "
                                    visible: layersList.currentIndex + 1 !== layersList.count

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            menu.close();
                                            const  modelIndex = drawOrderModel.mappedIndex(index);
                                            layerListModel.move(modelIndex, modelIndex - 1);
                                        }
                                    }
                                }

                                Label {
                                    id: moveDownLabel
                                    text: "  Move up  "
                                    visible: layersList.currentIndex !== 0

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            menu.close();
                                            const  modelIndex = drawOrderModel.mappedIndex(index);
                                            layerListModel.move(modelIndex, modelIndex + 1);
                                        }
                                    }
                                }

                                Label {
                                    text: "  Remove  "

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            menu.close();
                                            const  modelIndex = drawOrderModel.mappedIndex(index);
                                            deletedLayerListModel.push(layerListModel.get(modelIndex));
                                            deletedLayersList.model = deletedLayerListModel
                                            layerListModel.remove(modelIndex);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Label {
            text: "Deleted layers"
            visible: deletedLayersList.count > 0
            font {
                pixelSize: 14
                bold: true
            }
        }

        // Declare the ListView, which will display the list of files
        ListView {
            id: deletedLayersList
            height: contentHeight
            visible: deletedLayersList.count > 0
            width: 200
            interactive: true
            clip: true
            spacing: 5
            model: deletedLayerListModel

            delegate: Component {
                RowLayout {
                    width: parent.width
                    height: 40
                    spacing: 5

                    Label {
                        text: modelData.name
                        Layout.leftMargin: 5
                    }

                    Image {
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 25
                        Layout.preferredWidth: 25
                        source: "qrc:/Samples/Layers/ManageOperationalLayers/menu.png"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                layersList.currentIndex = index;
                                addMenu.open();
                            }
                        }

                        // add menu
                        Menu {
                            id: addMenu
                            width: 125

                            Column {
                                width: parent.width
                                spacing: 10

                                Label {
                                    text: "  Add to map  "

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            addMenu.close();
                                            layerListModel.append(deletedLayersList.model[index])
                                            deletedLayerListModel.splice(index, 1);
                                            deletedLayersList.model = deletedLayerListModel;
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
