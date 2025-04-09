// [WriteFile Name=CreateKmlMultiTrack, Category=EditData]
// [Legal]
// Copyright 2025 Esri.
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
import Esri.Samples
import QtQuick.Layouts

Item {
    MapView {
        id: view
        width: parent.width
        height: parent.height * 0.8
        Component.onCompleted: {
            // Set the focus on MapView to initially enable keyboard navigation
            forceActiveFocus();
        }
    }
    Rectangle {
        width: parent.width
        height: parent.height * 0.2
        anchors.top: view.bottom
        Loader {
            anchors.fill: parent
            sourceComponent: multiTrackModel.isShowTracksFromFileEnabled ? trackBrowseOptions : trackSimulationOptions
        }
    }

    // Track Browse Options Component
    Component {
        id: trackSimulationOptions
        ColumnLayout {
            anchors.fill: parent
            Component.onCompleted : multiTrackModel.startNavigation()

            // Spacer
            Item {
                height: 10
            }

            Text {
                visible: multiTrackModel.isRecordingTrack
                text: multiTrackModel.isRecordingTrack ? qsTr("Recording KML track. Elements added = ") + multiTrackModel.elementsCount : ""
                font.pixelSize: 18
                font.bold: true
                color: "red"
                Layout.alignment: Qt.AlignCenter
            }

            Text {
                text: qsTr("Number of tracks in Multitrack: ") + multiTrackModel.multiTrackCount
                font.pixelSize: 15
                font.bold: true
                Layout.alignment: Qt.AlignCenter
            }

            RowLayout {
                spacing: 0
                Layout.bottomMargin: 30
                Image {
                    sourceSize.width: 40
                    sourceSize.height: 40
                    Layout.leftMargin: 30
                    source: "qrc:/Samples/EditData/CreateKmlMultiTrack/" + (enabled ? "point.png" : "disabled-point.png")
                    enabled: multiTrackModel.isRecenterButtonEnabled
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            multiTrackModel.recenter()
                        }
                    }
                }

                // Spacer
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: startRecordingButton
                    text: multiTrackModel.isRecordingTrack ? qsTr("Stop Recording") : qsTr("Record Track(s)")
                    background: Rectangle {
                        color: "purple"
                        radius: 20
                    }
                    contentItem: Text {
                        text: startRecordingButton.text
                        font.pixelSize: 15
                        font.bold: true
                        color: "white"
                    }
                    onClicked: {
                        if (multiTrackModel.isRecordingTrack) {
                            multiTrackModel.stopRecordingKmlTrack();
                        } else {
                            multiTrackModel.startRecordingKmlTrack();
                        }
                    }
                }

                // Spacer
                Item {
                    Layout.fillWidth: true
                }

                Image {
                    sourceSize.width: 30
                    sourceSize.height: 30
                    Layout.rightMargin: 30
                    enabled: !multiTrackModel.isRecordingTrack
                    source: "qrc:/Samples/EditData/CreateKmlMultiTrack/" + (enabled ? "save.png" : "disabled-save.png")
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            multiTrackModel.exportKmlMultiTrack();
                        }
                    }
                }
            }
        }
    }

    // Track Browse Options Component
    Component {
        id: trackBrowseOptions
        ColumnLayout {
            anchors.fill: parent
            Component.onCompleted : multiTrackModel.loadLocalKmlFile()
            Text {
                width: parent.width
                text: qsTr("Browse KML Tracks")
                font.pixelSize: 18
                font.bold: true
                color: "black"
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            }

            RowLayout {
                Layout.alignment: Qt.AlignTop | Qt.AlignCenter
                // Dropdown Menu for Tracks
                ComboBox {
                    id: trackDropdown
                    height: 50
                    model: multiTrackModel.trackGeometriesCount
                    onCurrentIndexChanged: {
                        multiTrackModel.previewKmlTrack(currentIndex)
                    }
                }

                // Reset Button
                Button {
                    onClicked: multiTrackModel.reset()
                    background: Rectangle {
                        color: "purple"
                        radius: 10
                    }
                    contentItem: Text {
                        text: qsTr("Reset")
                        font.pixelSize: 15
                        font.bold: true
                        color: "white"
                    }
                }
            }
        }
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    CreateKmlMultiTrackSample {
        id: multiTrackModel
        mapView: view
    }
}
