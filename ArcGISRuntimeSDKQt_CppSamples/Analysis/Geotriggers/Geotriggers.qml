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
        onSectionInfoChanged: {
            console.log(focusSection);
            console.log(nearbySections);
            if (!(nearbySections.includes(focusSection))) {
                focusSection = "";
            }
        }
    }

    Control {
        id: nearbyFeatureButtons
        anchors.right: parent.right
        padding: 10
        background: Rectangle {
            color: "white"
        }
        contentItem: Column {
            id: column
            width: 200

            Text {
                text: "nearby features"
            }

            //            Repeater {
            //                model: sampleModel.nearbySections
            /*delegate: */Grid {
                width: 200
                columns: 2
                flow: Qt.LeftToRight
                spacing: 10
                Button {
                    text: "Desert"
                    onClicked: {
                        console.log(focusSection);
                        if (focusSection === text)
                            focusSection = "";
                        else {
                            focusSection = text;
                            sectionNameTextBox.text = focusSection;
                            img.source = sampleModel.sectionImgUrl[focusSection];
                            desc.text = sampleModel.sectionDesc[focusSection];
                        }
                        console.log(focusSection);
                    }
                }
                Button {
                    text: "Meadow"
                    onClicked: {
                        console.log(focusSection);
                        if (focusSection === text)
                            focusSection = "";
                        else {
                            focusSection = text;
                            sectionNameTextBox.text = focusSection;
                            img.source = sampleModel.sectionImgUrl[focusSection];
                            desc.text = sampleModel.sectionDesc[focusSection];
                        }
                        console.log(focusSection);
                    }
                }
                Button {
                    text: "Redwood"
                    onClicked: {
                        console.log(focusSection);
                        if (focusSection === text)
                            focusSection = "";
                        else {
                            focusSection = text;
                            sectionNameTextBox.text = focusSection;
                            img.source = sampleModel.sectionImgUrl[focusSection];
                            desc.text = sampleModel.sectionDesc[focusSection];
                        }
                        console.log(focusSection);
                    }
                }
                Button {
                    text: "Canyon"
                    onClicked: {
                        console.log(focusSection);
                        if (focusSection === text)
                            focusSection = "";
                        else {
                            focusSection = text;
                            sectionNameTextBox.text = focusSection;
                            img.source = sampleModel.sectionImgUrl[focusSection];
                            desc.text = sampleModel.sectionDesc[focusSection];
                        }
                        console.log(focusSection);
                    }
                }
            }
            //}
    }
        Control {
            anchors {
                top: nearbyFeatureButtons.bottom
                right: parent.right
            }

            padding: 10
            background: Rectangle {
                color: "white"
            }
            height: sectionInfoColumn.height/2 - nearbyFeatureButtons.height

            visible: true
            anchors.topMargin: 0
            contentItem: ScrollView {
                Column {
                    id: sectionInfoColumn
                    Image {
                        id: img
                        source: "qrc:/../../../../../../../../Desktop/boulder_jw_bd_10936.jpg"//sampleModel.sectionImgUrl[focusSection] ? sampleModel.sectionImgUrl[focusSection] : ""
                        fillMode: Image.PreserveAspectFit
                        width:200
                    }

                    Text {
                        id: sectionNameTextBox
                        text: "LOREM IPSUM"
                        font {
                            bold: true
                            pointSize: 16
                        }
                        wrapMode: Text.WordWrap
                        width: 200
                    }

                    Text {
                        id: desc
                        text: "Leverage agile frameworks to provide a robust synopsis for high level overviews. Iterative approaches to corporate strategy foster collaborative thinking to further the overall value proposition. Organically grow the holistic world view of disruptive innovation via workplace diversity and empowerment.

Bring to the table win-win survival strategies to ensure proactive domination. At the end of the day, going forward, a new normal that has evolved from generation X is on the runway heading towards a streamlined cloud solution. User generated content in real-time will have multiple touchpoints for offshoring.

Capitalize on low hanging fruit to identify a ballpark value added activity to beta test. Override the digital divide with additional clickthroughs from DevOps. Nanotechnology immersion along the information highway will close the loop on focusing solely on the bottom line.

Podcasting operational change management inside of workflows to establish a framework. Taking seamless key performance indicators offline to maximise the long tail. Keeping your eye on the ball while performing a deep dive on the start-up mentality to derive convergence on cross-platform integration."//sampleModel.sectionDesc[focusSection] ? sampleModel.sectionDesc[focusSection] : ""
                        wrapMode: Text.WordWrap
                        width: 200
                    }

                }
            }

        }
    }
}
