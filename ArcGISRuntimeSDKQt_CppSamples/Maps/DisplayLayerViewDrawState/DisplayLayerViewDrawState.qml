// [WriteFile Name=DisplayLayerViewDrawState, Category=Maps]
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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

DisplayLayerViewDrawStateSample {
    id: displayLayerView
    width: 800
    height: 600

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Rectangle {
            anchors {
                bottom: mapView.attributionTop
                horizontalCenter: parent.horizontalCenter
                margins: 5
            }
            border.color: "black"
            radius: 10
            height: childrenRect.height
            width: childrenRect.width
            opacity: 0.95
            GridLayout {
                id: gridLayout
                flow: GridLayout.TopToBottom
                rows: statusModel.length
                Repeater {
                    model: statusModel
                    delegate: Text {
                        text: model.modelData.name
                        horizontalAlignment: Text.AlignLeft
                        Layout.margins: 5
                    }
                }
                Repeater {
                    model: statusModel
                    delegate: Text {
                        text: model.modelData.status
                        color: "steelblue"
                        horizontalAlignment: Text.AlignRight
                        Layout.margins: 5
                    }
                }
            }
        }
    }
}
