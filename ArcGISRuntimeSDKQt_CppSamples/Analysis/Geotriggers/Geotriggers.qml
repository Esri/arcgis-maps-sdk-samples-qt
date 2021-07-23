// [WriteFile Name=Geotriggers, Category=Analysis]
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
import Esri.Samples 1.0

Item {

    property var sectionInfo: sampleModel.sectionInfo
    property var focusSection: ""

    // add a mapView component
    MapView {
        id: view
        anchors.fill: parent
    }

    // Declare the C++ instance which creates the scene etc. and supply the view
    GeotriggersSample {
        id: sampleModel
        mapView: view
    }

    Control {
        anchors.right: parent.right
        padding: 10
        background: Rectangle {
            color: "white"
        }
        contentItem: Column {
            width: 200

            Text {
                text: "nearby features"
            }

            Repeater {
                model: sampleModel.nearbySections
                delegate: Column {
                    width: 200
                    Button {
                        text: modelData
                        onClicked: {
                            console.log(focusSection);
                            if (focusSection === modelData)
                                focusSection = "";
                            else {
                                focusSection = modelData;
                                sectionNameTextBox.text = focusSection;
                                img.source = sampleModel.sectionImgUrl[focusSection];
                                desc.text = sampleModel.sectionDesc[focusSection];
                            }
                            console.log(focusSection);
                        }
                    }
                }
            }
            Image {
                id: img
                source: sampleModel.sectionImgUrl[focusSection] ? sampleModel.sectionImgUrl[focusSection] : ""
                width: 200
                fillMode: Image.PreserveAspectFit
                visible: focusSection !== ""
            }

            Text {
                id: sectionNameTextBox
                text: ""
                width: 200
                wrapMode: Text.WordWrap
                visible: focusSection !== ""
            }

            Text {
                id: desc
                text: sampleModel.sectionDesc[focusSection] ? sampleModel.sectionDesc[focusSection] : ""
                width: 200
                wrapMode: Text.WordWrap
                visible: focusSection !== ""
            }
        }
    }

}
