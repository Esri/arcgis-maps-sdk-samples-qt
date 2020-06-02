// [WriteFile Name=FeatureCollectionLayerFromPortal, Category=Layers]
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
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import Esri.Samples 1.0

Item {

    MessageDialog {
          id: messageDialog
          title: "Feature collection"
//          text:
          visible: false
          onRejected: {
              visible = false;
          }
      }

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent

        Rectangle {
            id: backgroundRect
            anchors {
                top: parent.top
                left: parent.left
                margins: 10
            }
            width: childrenRect.width
            height: childrenRect.height
            color: "lightgrey"

            GridLayout {
                rows: 3
                columns: 1
                rowSpacing: 0

                RowLayout {
                    Text {
                        text: "Enter item ID and click button"
                        font.bold: true
                    }
                }
                RowLayout {
                    TextField {
                        id: textfield
                        text: "32798dfad17942858d5eef82ee802f0b"
//                        onEditingFinished: console.log("editing finished");
                        onTextChanged: console.log("changed");
                        onTextEdited: console.log("edited");
                    }
                }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Button {
                        text: "Open from portal item"
                        onClicked: model.openFeatureCollection(textfield.text);
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    FeatureCollectionLayerFromPortalSample {
        id: model
        mapView: view
    }
}
