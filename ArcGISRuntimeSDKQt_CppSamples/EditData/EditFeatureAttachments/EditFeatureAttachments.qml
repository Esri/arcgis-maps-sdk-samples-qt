// [WriteFile Name=EditFeatureAttachments, Category=EditData]
// [Legal]
// Copyright 2015 Esri.

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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import Esri.Samples 1.0
import Esri.ArcGISExtras 1.1

EditFeatureAttachmentsSample {
    id: editAttachmentsSample
    width: 800
    height: 600

    property double scaleFactor: System.displayScaleFactor
    property var featAttributes: ["Destroyed", "Major", "Minor", "Affected", "Inaccessible"]

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent
        objectName: "mapView"
    }

    onFeatureSelected: {
        // show the callout
        callout.x = editAttachmentsSample.screenX;
        callout.y = editAttachmentsSample.screenY;
        callout.visible = true;
    }

    onHideWindow: {
        // hide the callout
        callout.visible = false;
        attachmentWindow.visible = false;
    }

    // map callout window
    Rectangle {
        id: callout
        width: col.width + (10 * scaleFactor) // add 10 for padding
        height: 60 * scaleFactor
        radius: 5
        border {
            color: "lightgrey"
            width: .5
        }
        visible: false

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
        }

        Column {
            id: col
            anchors {
                top: parent.top
                left: parent.left
                margins: 5 * scaleFactor
            }
            spacing: 10

            Row {
                spacing: 10

                Text {
                    text: editAttachmentsSample.featureType
                    font.pixelSize: 18 * scaleFactor
                }

                Rectangle {
                    radius: 100
                    width: 22 * scaleFactor
                    height: width
                    color: "transparent"
                    border.color: "blue"
                    antialiasing: true

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "i"
                        font.pixelSize: 18 * scaleFactor
                        color: "blue"
                    }

                    // create a mouse area over the (i) text to open the update window
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            attachmentWindow.visible = true;
                        }
                    }
                }
            }

            Row {
                spacing: 10

                Text {
                    text: editAttachmentsSample.attachmentModel === null ? "" : "Number of attachments: %1".arg(editAttachmentsSample.attachmentCount)
                    font.pixelSize: 12 * scaleFactor
                }
            }
        }
    }

    // attachment window
    Rectangle {
        id: attachmentWindow
        anchors.centerIn: parent
        height: 200 * scaleFactor
        width: 250 * scaleFactor
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
            height: 40 * scaleFactor
            color: "transparent"

            Text {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    margins: 10 * scaleFactor
                }

                text: "Attachments"; font {bold: true; pixelSize: 20 * scaleFactor;}
            }

            Row {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: 10 * scaleFactor
                }
                spacing: 15
                Text {
                    text: "+"; font {bold: true; pixelSize: 40 * scaleFactor;} color: "green"

                    // open a file dialog whenever the add button is clicked
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            fileDialog.open();
                        }
                    }
                }
                Text {
                    text: "-"; font {bold: true; pixelSize: 40 * scaleFactor;} color: "red"

                    // make sure an item is selected and if so, delete it from the service
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (attachmentsList.currentIndex === -1)  {
                                msgDialog.text = "Please first select an attachment to delete.";
                                msgDialog.open();
                            } else {
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
                margins: 10 * scaleFactor
            }
            clip: true
            spacing: 5
            // set the model equal to the C++ attachment list model property
            model: editAttachmentsSample.attachmentModel
            // create the delegate to specify how the view is arranged
            delegate: Item {
                height: 45* scaleFactor
                width: parent.width
                clip: true

                // show the attachment name
                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }
                    text: name
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 16 * scaleFactor
                }

                // show the attachment's URL if it is an image
                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                    }
                    width: 44 * scaleFactor
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
        onAccepted: {
            // add the attachment to the feature table
            fileInfo.url = fileDialog.fileUrl;
            // Call invokable C++ method to add an attachment to the model
            editAttachmentsSample.addAttachment(fileDialog.fileUrl, "application/octet-stream", fileInfo.fileName);
        }
    }

    MessageDialog {
        id: msgDialog
    }

    // file info used for obtaining the file name
    FileInfo {
        id: fileInfo
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border {
            width: 0.5 * scaleFactor
            color: "black"
        }
    }
}
