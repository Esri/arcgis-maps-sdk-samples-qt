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
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

DisplayLayerViewDrawStatusSample {
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
            width: parent.width * 0.25
            opacity: 0.95

            ListView {
                id: listView
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                model: statusModel
                delegate: Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: childrenRect.height

                    RowLayout {
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: 10
                        }
                        Text {
                            text: name
                            horizontalAlignment: Text.AlignLeft
                            Layout.fillWidth: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                        }
                        Text {
                            text: status
                            color: "steelblue"
                            horizontalAlignment: Text.AlignRight
                            Layout.fillWidth: true
                            Layout.topMargin: 10
                            Layout.bottomMargin: 10
                        }
                    }
                }
            }
        }
    }
}
