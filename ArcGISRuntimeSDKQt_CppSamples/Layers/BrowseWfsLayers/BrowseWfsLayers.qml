// [WriteFile Name=BrowseWfsLayers, Category=Geometry]
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
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Esri.Samples 1.0

Item {

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
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

            Row {
                Layout.margins: 3
                CheckBox {
                    id: axisOrderBox
                    checked: false
                }
                Text {
                    text: qsTr("Swap Coordinate Order")
                    height: axisOrderBox.height
                    verticalAlignment: Text.AlignVCenter
                    color: "#ffffff"
                }
            }

            ComboBox {
                id: layersComboBox
                model: browseWfsLayersSampleModel.layerInfoTitleListModel
                Layout.fillWidth: true
                Layout.margins: 3
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                id: loadSelectedLayerBtn
                text: qsTr("Load Selected Layer")
                Layout.fillWidth: true
                Layout.margins: 3
                onClicked: browseWfsLayersSampleModel.createWfsFeatureTable(layersComboBox.currentIndex, axisOrderBox.checked);
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    BrowseWfsLayersSample {
        id: browseWfsLayersSampleModel
        mapView: view
    }
}
