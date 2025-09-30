// [WriteFile Name=AddVectorTiledLayerFromCustomStyle, Category=Layers]
// [Legal]
// Copyright 2025 Esri.

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

import QtQuick
import QtQuick.Controls
import Esri.Samples

Item {
    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Component.onCompleted: {
            // Set and keep the focus on MapView to enable keyboard navigation
            forceActiveFocus();
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    AddVectorTiledLayerFromCustomStyleSample {
        id: model
        mapView: view
    }

    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 5
        }
        width: childrenRect.width
        height: childrenRect.height
        MouseArea {
            anchors.fill: parent
            onClicked: mouse => mouse.accepted = true
            onWheel: wheel => wheel.accepted = true
        }
        Column {
            id: settingsColumn
            spacing: 5
            padding: 15
            Text {
                id: selectStyle
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 16
                text: "Select style"
            }
            ComboBox {
                id: layerStyle
                anchors.horizontalCenter: parent.horizontalCenter
                width: 250
                model: model.styleNames

                onCurrentTextChanged: model.setStyle(currentText);
            }
        }
    }
}
