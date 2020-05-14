// [WriteFile Name=DisplayUtilityAssociations, Category=UtilityNetwork]
// [Legal]
// Copyright 2020 Esri.

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

        Control {
            background: Rectangle {
                border.color: "black"
                border.width: 1
            }
            padding: 5
            visible: model.attachmentSymbolUrl !== "" && model.connectivitySymbolUrl !== ""
            anchors {
                top: parent.top
                left: parent.left
                margins: 20
            }

            contentItem: GridLayout {
                id: grid
                columns: 2
                anchors.horizontalCenter: parent.horizontalCenter
                Layout.fillWidth: true
                Label {
                    text: "Utility association types"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }

                Image {
                    id: attachmentImage
                    source: model.attachmentSymbolUrl
                    fillMode: Image.PreserveAspectFit
                }
                Label {
                    id: attachmentLabel
                    text: "Attachment symbol"
                    visible: model.attachmentSymbolUrl !== "" && model.connectivitySymbolUrl !== ""
                }

                Image {
                    id: connectivityImage
                    source: model.connectivitySymbolUrl
                }
                Label {
                    id: connectivityLabel
                    text: "Connectivity symbol"
                    visible: model.attachmentSymbolUrl !== "" && model.connectivitySymbolUrl !== ""
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    DisplayUtilityAssociationsSample {
        id: model
        mapView: view
    }
}
