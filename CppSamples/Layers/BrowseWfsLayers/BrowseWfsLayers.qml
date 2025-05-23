// [WriteFile Name=BrowseWfsLayers, Category=Layers]
// [Legal]
// Copyright 2019 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
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
    readonly property bool swapAxis: true

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }

    BusyIndicator {
        id: loadingIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        running: browseWfsLayersSampleModel.isLoading;
    }

    Rectangle {
        anchors {
            margins: 5
            right: parent.right
            top: parent.top
        }
        width: childrenRect.width
        height: childrenRect.height
        color: "#000000"
        opacity: .75
        radius: 5

        ColumnLayout {
            Text {
                text: qsTr("Pick a WFS Layer")
                Layout.margins: 3
                Layout.alignment: Qt.AlignHCenter
                color: "#ffffff"
            }

            ComboBox {
                id: layersComboBox
                model: browseWfsLayersSampleModel.layerInfoTitleListModel
                Layout.fillWidth: true
                Layout.margins: 3
                Layout.alignment: Qt.AlignHCenter

                // Add a background to the ComboBox
                Rectangle {
                    anchors.fill: parent
                    radius: 10
                    // Make the rectangle visible if a dropdown indicator exists
                    // An indicator only exists if a theme is set
                    visible: parent.indicator
                    border.width: 1
                }
            }

            Button {
                id: loadSelectedLayerBtn
                text: qsTr("Load Selected Layer")
                Layout.fillWidth: true
                Layout.margins: 3
                onClicked: browseWfsLayersSampleModel.createWfsFeatureTable(layersComboBox.currentIndex, !swapAxis);
            }

            Button {
                id: swapAxisOrder
                text: qsTr("Swap Coordinate Order")
                Layout.margins: 3
                Layout.fillWidth: true
                onClicked:{
                    browseWfsLayersSampleModel.createWfsFeatureTable(layersComboBox.currentIndex, swapAxis);
                }
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    BrowseWfsLayersSample {
        id: browseWfsLayersSampleModel
        mapView: view
    }
}
