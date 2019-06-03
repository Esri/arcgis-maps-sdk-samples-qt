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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    ManageOperationalLayersSample {
        id: sample
        mapView: view
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
            model: sample.layerListModel
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
                                            sample.moveLayerDown(index);
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
                                            sample.moveLayerUp(index);
                                        }
                                    }
                                }

                                Label {
                                    text: "  Remove  "

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            menu.close();
                                            sample.removeLayer(index);
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
            visible: deletedLayersListView.count > 0
            font {
                pixelSize: 14
                bold: true
            }
        }

        // Declare the ListView, which will display the list of files
        ListView {
            id: deletedLayersListView
            height: contentHeight
            visible: deletedLayersListView.count > 0
            width: 200
            interactive: true
            clip: true
            spacing: 5
            model: sample.deletedLayersList

            delegate: Component {
                RowLayout {
                    width: parent.width
                    height: 40
                    spacing: 5

                    Label {
                        text: modelData
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
                                            sample.addLayer(index);
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
