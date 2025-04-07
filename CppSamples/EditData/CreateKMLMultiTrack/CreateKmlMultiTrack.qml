// [WriteFile Name=NavigateRoute, Category=EditData]
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
        height: parent.height * 0.8
        width: parent.width
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
            sourceComponent: multiTrackModel.showTracksFromFileEnabled ? trackBrowseOptions : trackSimulationOptions
        }
    }

    // Track Browse Options Component
    Component {
        id: trackSimulationOptions
        ColumnLayout {
            anchors.fill: parent
            Component.onCompleted : multiTrackModel.startNavigation()
            Text {
                height: 25
                text: multiTrackModel.isRecordingTrack ? "Recording KML track. Elements added = " + multiTrackModel.elementsCount: ""
                font.pixelSize: 18
                font.bold: true
                color: "red"
                Layout.alignment: Qt.AlignCenter
            }

            Text {
                text: "Number of track in Multitrack: " + multiTrackModel.multiTrackCount
                font.pixelSize: 15
                Layout.alignment: Qt.AlignCenter
            }

            RowLayout {
                width: parent.width
                Layout.alignment: Qt.AlignCenter
                //spacing: 30
                Image {
                    width: 32
                    height: 32
                    source: "point-32.png"
                    enabled: multiTrackModel.isRecenterButtonEnabled
                    Layout.alignment: Qt.AlignLeft
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            onClicked: multiTrackModel.recenter()
                        }
                    }
                }

                // Spacer
                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: startRecordingButton
                    height: 200
                    text: multiTrackModel.isRecordingTrack ? "Stop Recording" : "Record Track(s)";
                    background: Rectangle {
                        color: "purple"
                        radius: 20
                    }
                    contentItem: Text {
                        text: startRecordingButton.text
                        font.pixelSize: 20
                        font.bold: true
                        color: "white"
                    }
                    onClicked: {
                        if (multiTrackModel.isRecordingTrack ) {
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
                    width: 32
                    height: 32
                    Layout.alignment: Qt.AlignRight
                    enabled: !multiTrackModel.isRecordingTrack
                    source: "save-32.png"
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
            //spacing: 12
            anchors.fill: parent
            Component.onCompleted : multiTrackModel.loadLocalKmlFile()
            Text {
                width: parent.width
                text: "Browse KML Tracks"
                font.pixelSize: 18
                font.bold: true
                color: "red"
                Layout.alignment: Qt.AlignCenter
            }
            RowLayout {
                Layout.alignment: Qt.AlignCenter
                // Dropdown Menu for Tracks
                ComboBox {
                    id: trackDropdown
                    height: 50
                    model: multiTrackModel.trackGeometriesCount
                    onCurrentIndexChanged: {
                        multiTrackModel.previewKmlTrack(currentIndex)
                    }

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

                // Reset Button
                Button {
                    width: 100
                    height: 50
                    onClicked: multiTrackModel.reset()
                    background: Rectangle {
                        color: "purple"
                        radius: 10
                    }
                    contentItem: Text {
                        text: "Reset"
                        font.pixelSize: 20
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
