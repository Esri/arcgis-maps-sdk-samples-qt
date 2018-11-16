// [WriteFile Name=EditFeatureAttachments, Category=EditData]
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
import Qt.labs.platform 1.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2
import Esri.Samples 1.0
import Esri.ArcGISRuntime.Toolkit.Controls 100.5

EditFeatureAttachmentsSample {
    id: editAttachmentsSample

    

    property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"

        Callout {
            id: callout
            borderColor: "lightgrey"
            borderWidth: 1
            calloutData: editAttachmentsSample.calloutData
            leaderPosition: leaderPositionEnum.Automatic
            onAccessoryButtonClicked: {
                attachmentWindow.visible = true;
            }
        }
    }

    onFeatureSelected: {
        // show the callout
        callout.showCallout();
    }

    onHideWindow: {
        // hide the callout
        if (callout.visible)
            callout.dismiss();
        attachmentWindow.visible = false;
    }

    // attachment window
    Rectangle {
        id: attachmentWindow
        anchors.centerIn: parent
        height: 200
        width: 250
        visible: false
        radius: 10
        color: "lightgrey"
        border.color: "darkgrey"
        opacity: 0.90
        clip: true

        // accept mouse events so they do not propogate down to the map
        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            id: titleText
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: 40
            color: "transparent"

            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    margins: 10
                }

                text: "Attachments"; font {bold: true; pixelSize: 20;}
            }

            Row {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 10
                }
                spacing: 15
                Text {
                    text: "+"; font {bold: true; pixelSize: 40;} color: "green"

                    // open a file dialog whenever the add button is clicked
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                }
                Text {
                    text: "-"; font {bold: true; pixelSize: 40;} color: "red"

                    // make sure an item is selected and if so, delete it from the service
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (attachmentsList.currentIndex !== -1)  {
                                // Call invokable C++ method to add an attachment to the model
                                editAttachmentsSample.deleteAttachment(attachmentsList.currentIndex);
                            }
                        }
                    }
                }
            }
        }

        ListView {
            id: attachmentsList
            anchors {
                left: parent.left
                right: parent.right
                top: titleText.bottom
                bottom: parent.bottom
                margins: 10
            }
            clip: true
            spacing: 5
            // set the model equal to the C++ attachment list model property
            model: editAttachmentsSample.attachmentModel
            // create the delegate to specify how the view is arranged
            delegate: Item {
                height: 45
                width: parent.width
                clip: true

                // show the attachment name
                Text {
                    id: label
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: attachment.left
                    }
                    text: name
                    wrapMode: Text.WrapAnywhere
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    font.pixelSize: 16
                }

                // show the attachment's URL if it is an image
                Image {
                    id: attachment
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    width: 44
                    height: width
                    fillMode: Image.PreserveAspectFit
                    source: attachmentUrl
                }

                MouseArea {
                    id: itemMouseArea
                    anchors.fill: parent
                    onClicked: {
                        attachmentsList.currentIndex = index;
                    }
                }
            }

            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                height: attachmentsList.currentItem.height
                color: "lightsteelblue"
            }
        }
    }

    // file dialog for selecting a file to add as an attachment
    FileDialog {
        id: fileDialog
        folder: {
            var locs = StandardPaths.standardLocations(StandardPaths.PicturesLocation)
            return locs.length > 0 ? locs.last() : "";
        }
        onAccepted: {
            // Call invokable C++ method to add an attachment to the model
            editAttachmentsSample.addAttachment(fileDialog.file, "application/octet-stream");
        }
    }
}
